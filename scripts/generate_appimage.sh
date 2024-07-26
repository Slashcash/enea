#!/bin/bash

# Default paths (script's parent path)
DEFAULT_SOURCE_DIR="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"
DEFAULT_OUTPUT_DIR="$DEFAULT_SOURCE_DIR"
DEFAULT_ENEA_ARCH="x86_64"
DEFAULT_BUILD_DEPS="missing"

# temporary folders
base_temp_dir="/tmp/enea_appimage"
app_dir="$base_temp_dir/app"
downloads_dir="$base_temp_dir/downloads"

# Function to display usage help
function display_help {
  echo "Usage: $(basename "$0") [-s <source_folder>] [-o <output_directory>] [-a <architecture>]"
  echo "  -s  Specify the source folder for enea"
  echo "  -o  Specify the output path"
  echo "  -a  Specify build architecture (either x86_64, aarch64 or armv7hf)"
  echo "  -b  Build all dependencies from scratch"
  echo "  -h  Display this help message"
}

# Function to clean up temporary folders
function cleanup {
  rm $base_temp_dir/* 2> /dev/null || true
  rm -rf "$app_dir"
  rm -rf "$downloads_dir"
}

# Trap function to ensure cleanup is called on exit
trap cleanup EXIT

# Sourcing common scripts
source $(dirname $0)/common.sh

# Parse command line options
while getopts "s:o:a:hb" opt; do
  case "$opt" in
    s)
      SOURCE_DIR=$(realpath "$OPTARG")
      ;;
    o)
      OUTPUT_DIR=$(realpath "$OPTARG")
      ;;
    a)
      ENEA_ARCH="$OPTARG"
      ;;
    h)
      display_help
      exit 0
      ;;
    b)
      BUILD_DEPS="*"
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
ENEA_ARCH=${ENEA_ARCH:-$DEFAULT_ENEA_ARCH}
BUILD_DEPS=${BUILD_DEPS:-$DEFAULT_BUILD_DEPS}

# Checking if the folder really contains enea source folder
if ! check_path_validity "$SOURCE_DIR/.env"; then
  exit 1
fi

# Checking if the output folder is writable
if ! is_path_writable "$OUTPUT_DIR"; then
  exit 1
fi

# Checking if build architecture is a suitable value
if [ "$ENEA_ARCH" == "x86_64" ]; then
  CONAN_ARCH="x86_64"
  SKIP_TESTS="False"
  PKGCONFIG_ARCH="x86_64-linux-gnu"
elif [ "$ENEA_ARCH" == "armv7hf" ]; then
  CONAN_ARCH="armv7hf"
  SKIP_TESTS="True"
  PKGCONFIG_ARCH="arm-linux-gnueabihf"
elif [ "$ENEA_ARCH" == "aarch64" ]; then
  CONAN_ARCH="armv8"
  SKIP_TESTS="True"
  PKGCONFIG_ARCH="aarch64-linux-gnu"
else
  echo "Error: build architecture must be either 'x86_64', 'aarch64' or 'armv7hf'."
  exit 1
fi

# Creating temporary folders
mkdir -p "$app_dir"
if ! is_path_writable "$app_dir"; then
  exit 1
fi

mkdir -p "$downloads_dir"
if ! is_path_writable "$downloads_dir"; then
  exit 1
fi

mkdir -p "$base_temp_dir"
if ! is_path_writable "$base_temp_dir"; then
  exit 1
fi

# Sourcing environment
. $SOURCE_DIR/.env

# Downloading the icon file
icon_url="https://enea.geniorio.it/$SOFTWARE_VERSION/icon/icon.png"
icon_path="$downloads_dir/icon.png"
if ! download_file "$icon_url" "$icon_path"; then
    exit 1
fi

# Downloading linux deploy
deploy_url="https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-static-x86_64.AppImage"
deploy_path="$downloads_dir/linuxdeploy"
if ! download_file "$deploy_url" "$deploy_path"; then
    exit 1
fi

# Setting linuxdeploy as executable and fixing magic byte in order to run it in a docker (https://github.com/linuxdeploy/linuxdeploy/issues/86)
chmod +x $deploy_path && sed -i 's|AI\x02|\x00\x00\x00|' $deploy_path

# Create enea.desktop file
desktop_path="$base_temp_dir/enea.desktop"
echo "[Desktop Entry]
Version=1.0
Type=Application
Icon=icon
Terminal=false
Exec=enea
Categories=Game;Emulator
Name=Enea" > "$desktop_path"

# Create AppRun file
launcher_path="$base_temp_dir/enea_launcher.sh"
echo '#!/bin/bash
show_help() {
    echo "Launch the Enea application with optional log level setting."
    echo ""
    echo "Options:"
    echo "  -l, --log LEVEL     Set log level to one of: trace, debug, info, warn, or error (default: info)"
    echo "  -h, --help          Show this help message"
}

SPDLOG_LEVEL="info"

while [[ $# -gt 0 ]]; do
key="$1"
case $key in
    -l|--log)
        if [ -z "$2" ]; then
            echo "Error: Missing argument for log level option."
            exit 1
        fi
        # Validate the log level argument
        case $2 in
            trace|debug|info|warn|error)
                SPDLOG_LEVEL=$2
                shift # Move to the next argument
                ;;
            *)
                echo "Error: Invalid log level. Valid values are trace, debug, info, warn or error."
                show_help
                exit 1
                ;;
        esac
        shift # Move to the next argument
        ;;
    -h|--help)
        show_help
        exit 0
        ;;
    *)
        echo "Error: Unknown option $1"
        show_help
        exit 1
        ;;
esac
done

export PATH="$APPDIR/usr/bin"
export SPDLOG_LEVEL

exec $APPDIR/usr/bin/enea' > "$launcher_path"

# Compiling enea
conan config install "$SOURCE_DIR/conan"
PKG_CONFIG_PATH="/usr/lib/$PKGCONFIG_ARCH/pkgconfig" \
conan build -pr:h linux-$ENEA_ARCH-gcc-11.3-release -pr:b linux-x86_64-gcc-11.3-host --build "$BUILD_DEPS" -c tools.build:skip_test=$SKIP_TESTS "$SOURCE_DIR"

# Check if building enea was successful
if [ $? -ne 0 ]; then
    echo "Error: building enea failed."
    exit 1
fi

advmame_package_folder="$(conan cache path advmame/$ADVMAME_VERSION:$(conan list -p "arch=$CONAN_ARCH" "advmame/$ADVMAME_VERSION:*" | grep -A1 packages | grep -v packages | sed 's/^ *//'))"
ENEA_EXEC="$SOURCE_DIR/build/Release/app/enea"
ADVMAME_EXEC="$advmame_package_folder/bin/advmame"

# Using linuxdeploy to generate app dir
appimage_name="Enea-$ENEA_ARCH.AppImage"
appimage_output="$OUTPUT_DIR/$appimage_name"
echo "Generating "$appimage_output""
export LDAI_OUTPUT="$appimage_output"
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|Slashcash|enea|latest|$appimage_name.zsync"
export NO_STRIP=1
cd $OUTPUT_DIR
$deploy_path --appimage-extract-and-run --appdir="$app_dir" --custom-apprun="$launcher_path" --executable="$ENEA_EXEC" --executable="$ADVMAME_EXEC" --icon-file="$icon_path" --desktop-file="$desktop_path" --output appimage

# Check if linuxdeploy was successful
if [ $? -ne 0 ]; then
    echo "Error: linuxdeploy failed."
    exit 1
fi

exit 0
