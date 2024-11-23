#!/bin/bash

# Default paths (script's parent path)
DEFAULT_SOURCE_DIR_ARG="-s $( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"
DEFAULT_OUTPUT_DIR_ARG="$DEFAULT_SOURCE_DIR_ARG"
DEFAULT_BUILD_DEPS_ARG=""
DEFAULT_GENERATE_UPDATE_INFO_ARG=""
DEFAULT_GPG_SIGNATURE_ID_ARG=""

# Function to display usage help
function display_help {
  echo "Usage: $(basename "$0") [OPTIONS] [-s <source_folder>] [-o <output_directory>] [-a <architecture>] [-g <gpg_id>] [-bu]"
  echo "  -s  Specify the source folder for enea"
  echo "  -o  Specify the output path"
  echo "  -a  Specify build architecture (either Linux-x86_64, Linux-aarch64 or Linux-armv7hf or Windows)"
  echo "  -g  Specify a gpg key ID that will be used to sign the packaged software (only available for Linux)"
  echo "  -b  Build all dependencies from scratch"
  echo "  -u  Embed update info into the packaged software (only availabe for Linux)"
  echo "  -h  Display this help message"
}

# Parse command line options
while getopts "s:o:g:a:hbu" opt; do
  case "$opt" in
    s)
      SOURCE_DIR_ARG="-s $(realpath "$OPTARG")"
      ;;
    o)
      OUTPUT_DIR_ARG="-o $(realpath "$OPTARG")"
      ;;
    a)
      ENEA_ARCH_ARG="$OPTARG"
      ;;
    g)
      GPG_SIGNATURE_ID_ARG="-g $OPTARG"
      ;;
    h)
      display_help
      exit 0
      ;;
    b)
      BUILD_DEPS_ARG="-b"
      ;;
    u)
      GENERATE_UPDATE_INFO_ARG="-u"
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

SOURCE_DIR_ARG=${SOURCE_DIR_ARG:-$DEFAULT_SOURCE_DIR_ARG}
OUTPUT_DIR_ARG=${OUTPUT_DIR_ARG:-$DEFAULT_OUTPUT_DIR_ARG}
BUILD_DEPS_ARG=${BUILD_DEPS_ARG:-$DEFAULT_BUILD_DEPS_ARG}
GENERATE_UPDATE_INFO_ARG=${GENERATE_UPDATE_INFO_ARG:-$DEFAULT_GENERATE_UPDATE_INFO_ARG}
GPG_SIGNATURE_ID_ARG=${GPG_SIGNATURE_ID_ARG:-$DEFAULT_GPG_SIGNATURE_ID_ARG}

# Checking if build architecture is a suitable value
if [ "$ENEA_ARCH_ARG" == "Linux-x86_64" ]; then
    ARCH_ARG="-a x86_64"
    SCRIPT="$(dirname "$0")/generate_appimage.sh"
elif [ "$ENEA_ARCH_ARG" == "Linux-armv7hf" ]; then
    ARCH_ARG="-a armv7hf"
    SCRIPT="$(dirname "$0")/generate_appimage.sh"
elif [ "$ENEA_ARCH_ARG" == "Linux-aarch64" ]; then
    ARCH_ARG="-a aarch64"
    SCRIPT="$(dirname "$0")/generate_appimage.sh"
elif [ "$ENEA_ARCH_ARG" == "Windows" ]; then
    ARCH_ARG="-a x86_64"
    SCRIPT="$(dirname "$0")/generate_windows_executable.sh"
else
  echo "Error: build architecture must be either 'Linux-x86_64', 'Linux-aarch64', 'Linux-armv7hf' or 'Windows'"
  exit 1
fi

$SCRIPT $ARCH_ARG $SOURCE_DIR_ARG $OUTPUT_DIR_ARG $BUILD_DEPS_ARG $GENERATE_UPDATE_INFO_ARG $GPG_SIGNATURE_ID_ARG
