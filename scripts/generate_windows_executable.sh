#!/bin/bash

DEFAULT_SOURCE_DIR="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"
DEFAULT_OUTPUT_DIR="$DEFAULT_SOURCE_DIR"
DEFAULT_BUILD_DEPS="missing"

# Function to display usage help
function display_help {
  echo "Usage: $(basename "$0") [OPTIONS] [-s <source_folder>] [-o <output_directory>] [-b]"
  echo "  -s  Specify the source folder for enea"
  echo "  -o  Specify the output path"
  echo "  -b  Build all dependencies from scratch"
  echo "  -h  Display this help message"
}

# Sourcing common scripts
source $(dirname $0)/common.sh

# Parse command line options
# Please note that we are parsing some unimplemented flags with a warning, this is just to provide
# a common interactive interface between this script and generate_appimage.sh
while getopts "s:o:g:a:hbu" opt; do
  case "$opt" in
    s)
      SOURCE_DIR=$(realpath "$OPTARG")
      ;;
    o)
      OUTPUT_DIR=$(realpath "$OPTARG")
      ;;
    a)
      echo "Warning: additional architecture not implemented for Windows, sticking with x86_64"
      ;;
    g)
      echo "Warning: GPG Signing not implemented for Windows, sticking up with unsigned executable"
      ;;
    h)
      display_help
      exit 0
      ;;
    b)
      BUILD_DEPS="*"
      ;;
    u)
      echo "Warning: Automatic update information not embeddable for Windows"
      ;;
    \?)
      display_help
      exit 1
      ;;
    :)
      echo "Invalid option: -$OPTARG"
      display_help
      exit 1
      ;;
  esac
done

SOURCE_DIR=${SOURCE_DIR:-$DEFAULT_SOURCE_DIR}
OUTPUT_DIR=${OUTPUT_DIR:-$DEFAULT_OUTPUT_DIR}
BUILD_DEPS=${BUILD_DEPS:-$DEFAULT_BUILD_DEPS}

# Checking if the folder really contains enea source folder
if ! check_path_validity "$SOURCE_DIR"; then
  exit 1
fi

# Checking if the output folder is writable
if ! is_path_writable "$OUTPUT_DIR"; then
  exit 1
fi

# Compiling enea
conan config install "$SOURCE_DIR/conan"
conan build -pr:h windows-x86_64-gcc-10.5-release -pr:b linux-x86_64-gcc-11.3-host --build "$BUILD_DEPS" -c tools.build:skip_test=True "$SOURCE_DIR"
