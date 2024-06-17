#!/bin/bash

# Default paths (script's parent path)
DEFAULT_SOURCE_DIR="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"
DEFAULT_OUTPUT_DIR="$DEFAULT_SOURCE_DIR"

# temporary folders
base_temp_dir="/tmp/enea_appimage"
app_dir="$base_temp_dir/app"
downloads_dir="$base_temp_dir/downloads"
logs_dir="$base_temp_dir/logs"

# Function to display usage help
function display_help {
  echo "Usage: $(basename "$0") [-s <source_folder>] [-o <output_directory>] [-a <executable>] [-e <executable>]"
  echo "  -s  Specify the source folder for enea"
  echo "  -o  Specify the output path"
  echo "  -a  Specify the advancemame executable"
  echo "  -e  Specify the enea executable"
  echo "  -h  Display this help message"
}

# Function to clean up temporary folders (keeping the logs folder just in case we need to analyze it)
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
while getopts "s:o:a:e:h" opt; do
  case "$opt" in
    s)
      SOURCE_DIR=$(realpath "$OPTARG")
      ;;
    o)
      OUTPUT_DIR=$(realpath "$OPTARG")
      ;;
    a)
      USER_ADVMAME_EXEC=$(realpath "$OPTARG")
      ;;
    e)
      USER_ENEA_EXEC=$(realpath "$OPTARG")
      ;;
    h)
      display_help
      exit 0
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

# Checking if the folder really contains enea source folder
if ! check_path_validity "$SOURCE_DIR/.env"; then
  exit 1
fi

# Checking if the output folder is writable
if ! is_path_writable "$OUTPUT_DIR"; then
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

mkdir -p "$logs_dir"
if ! is_path_writable "$logs_dir"; then
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

# Compiling advmame source in case user didn't pass a valid advmame executable
if [ -z "$USER_ADVMAME_EXEC" ]; then
    echo "Compiling advmame from source. This may take a very long time."
    advmame_log="$logs_dir/advmame.log"

    find "$SOURCE_DIR/conan_recipes" -name "conanfile.py" -execdir conan export . \; > /dev/null 2>&1
    conan create -pr:h $SOURCE_DIR/conan_profiles/linux-x86_64-gcc-11.3-release -pr:b $SOURCE_DIR/conan_profiles/linux-x86_64-gcc-11.3-host --build "missing" $(conan cache path advmame/$ADVMAME_VERSION) &> "$advmame_log"

    # Check if building enea was successful
    if [ $? -ne 0 ]; then
        echo "Error: building advmame failed. A log of the compilation process can be analyzed at "$advmame_log""
        exit 1
    fi

    advmame_package_folder="$(conan cache path advmame/$ADVMAME_VERSION:$(conan list "advmame/$ADVMAME_VERSION:*" | grep -A1 packages | grep -v packages | sed 's/^ *//'))"

    ADVMAME_EXEC="$advmame_package_folder/bin/advmame"
else
    ADVMAME_EXEC="$USER_ADVMAME_EXEC"
fi

echo "Using advancemame executable at $ADVMAME_EXEC"

# Compiling enea in case user didn't pass a valid enea executable
if [ -z "$USER_ENEA_EXEC" ]; then
    echo "Compiling enea from source. This may take a very long time."
    enea_log="$logs_dir/enea.log"

    find "$SOURCE_DIR/conan_recipes" -name "conanfile.py" -execdir conan export . \; > /dev/null 2>&1
    conan create -pr:h $SOURCE_DIR/conan_profiles/linux-x86_64-gcc-11.3-release -pr:b $SOURCE_DIR/conan_profiles/linux-x86_64-gcc-11.3-host --build "missing" -c tools.build:skip_test=true --name enea --version $SOFTWARE_VERSION "$SOURCE_DIR" &> "$enea_log"

    # Check if building enea was successful
    if [ $? -ne 0 ]; then
        echo "Error: building enea failed. A log of the compilation process can be analyzed at "$enea_log""
        exit 1
    fi

    enea_package_folder="$(conan cache path enea/$SOFTWARE_VERSION:$(conan list "enea/$SOFTWARE_VERSION:*" | grep -A1 packages | grep -v packages | sed 's/^ *//'))"

    ENEA_EXEC="$enea_package_folder/bin/enea"
else
    ENEA_EXEC="$USER_ENEA_EXEC"
fi

echo "Using enea executable at $ENEA_EXEC"

# Using linuxdeploy to generate app dir
appimage_name="Enea-x86_64.AppImage"
appimage_output="$OUTPUT_DIR/$appimage_name"
appimage_log="$logs_dir/appimage.log"
echo "Genearting "$appimage_output""
export LDAI_OUTPUT="$appimage_output"
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|Slashcash|enea|latest|$appimage_name.zsync"
cd $OUTPUT_DIR
$deploy_path --appimage-extract-and-run --appdir="$app_dir" --custom-apprun="$launcher_path" --executable="$ENEA_EXEC" --executable="$ADVMAME_EXEC" --icon-file="$icon_path" --desktop-file="$desktop_path" --output appimage &> "$appimage_log"

# Check if linuxdeploy was successful
if [ $? -ne 0 ]; then
    echo "Error: linuxdeploy failed. A log of the packing process can be analyzed at "$appimage_log""
    exit 1
fi

exit 0
