#!/bin/bash

# Default paths (script's parent path)
DEFAULT_SOURCE_DIR="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"
DEFAULT_OUTPUT_DIR="$DEFAULT_SOURCE_DIR"
DEFAULT_ENEA_ARCH="x86_64"
DEFAULT_BUILD_DEPS="never"
DEFAULT_GENERATE_UPDATE_INFO=false

# temporary folders
base_temp_dir="/tmp/enea_appimage"
app_dir="$base_temp_dir/app"
share_dir="$app_dir/usr/share"
bundled_roms_dir="$share_dir/enea/bundled_roms"
downloads_dir="$base_temp_dir/downloads"

declare -a found_deps

# Function to check if a string starts with any exclusion string
is_excluded() {
    # Exclusion list of libs that don't need to be included into the AppImage
    local exclusion_list=("libGL.so" "libSM.so" "libuuid.so" "libxcb-dri2.so" "libxcb-dri3.so"
                "libICE.so" "libX11-xcb.so" "libxcb.so" "libX11.so" "libstdc++.so"
                "libm.so" "libgcc_s.so" "libz.so" "ld-linux" "libc.so"
                "libGLX.so" "libGLdispatch.so")

    local string="$1"
    for exclude in "${exclusion_list[@]}"; do
        if [[ "$string" == "$exclude"* ]]; then
            return 0  # String starts with exclusion pattern
        fi
    done
    return 1  # String does not match any exclusion pattern
}

find_dependencies() {
    local file="$1"
    local search_path="$2"
    local -a deps
    local -a deps_to_process

    deps=( $( readelf -d "$file" | grep "NEEDED" | \
        grep -o -E "\[[^]]*\]" | grep -o -E "[^][]*" ) )

    local add_this_dep=true

    # always assume we've found $file and add it to the $found_deps list
    # if it's not there
    for found_dep in "${found_deps[@]}"
    do
        if [ "$found_dep" = "$(basename $file)" ]
        then
            add_this_dep=false
            break
        fi
    done

    # if $add_this_dep is true, go ahead and add to the found_deps list
    if $add_this_dep
    then
        found_deps+=("$(basename $file)")
    fi

    # for every dependency found by readelf (no path)
    for dep in "${deps[@]}"
    do
        local process_dep=true

        # if the basename of the file passed into the function is
        # this dep, skip processing altogether
        if [ "$dep" = "$(basename $file)" ]
        then
            break
        else
            # otherwise, if it's one of the 'found deps' don't process it
            for found_dep in "${found_deps[@]}"
            do
                if [ "$dep" = "$found_dep" ]
                then
                    process_dep=false
                    break
                fi
            done

            # it wasn't one of the 'found deps' so add
            # it to the found_deps list
            if $process_dep
            then
                found_deps+=($dep)
            fi
        fi

        # if we are supposed to process this dep
        if $process_dep
        then
            local file_path=

            # check each search path for a file named $dep underneath it
            dep_basename="$(echo $dep | sed 's/\.so.*$/.so/')"
            file_path=$( find "$search_path" -name "$dep_basename*" | head -n 1 )

            # if the $file_path is not empty, then we found a copy
            # of the file, place it the list of deps to be processed
            if [ -n "$file_path" ]
            then
                deps_to_process+=("$(realpath $file_path)")
            fi
        fi
    done

    # now, go through all of our $deps_to_process (with path)
    # and run "find_dependencies" on them
    for dep_to_process in "${deps_to_process[@]}"
    do
        find_dependencies "$dep_to_process" "$2"
    done
}

list_dependencies() {
    local executable="$1"
    local ld_search_path="$2"
    local append_variable=""

    find_dependencies "$executable" "$ld_search_path"

    # transforming dependencies into their real path
    for path_dep in "${found_deps[@]}"
    do
        if ! is_excluded "$path_dep"; then
            path_dep_basename="$(echo $path_dep | sed 's/\.so.*$/.so/')"
            real_path=$(realpath $(find "$2" -name "$path_dep_basename*" | head -n 1 ))

            if [ -n "$real_path" ]
            then

                append_variable+="--library=$(echo $real_path | sed 's/\(\.so\.[0-9]*\).*/\1/') "
                #cp "$(realpath $real_path)" "$copy_path/$(echo "$path_dep" | sed 's/\(\.so\.[0-9]*\).*/\1/')"
            fi
        fi
    done

    for variable in "${append_variable[@]}"
    do
      echo "$variable"
    done
}


# Function to display usage help
function display_help {
  echo "Usage: $(basename "$0") [OPTIONS] [-s <source_folder>] [-o <output_directory>] [-a <architecture>] [-g <gpg_id>] [-bu]"
  echo "  -s  Specify the source folder for enea"
  echo "  -o  Specify the output path"
  echo "  -a  Specify build architecture (either x86_64, aarch64 or armv7hf)"
  echo "  -g  Specify a gpg key ID that will be used to sign the AppImage"
  echo "  -b  Build all dependencies from scratch"
  echo "  -u  Embed update info into the AppImage"
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
while getopts "s:o:g:a:hbu" opt; do
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
    g)
      GPG_SIGNATURE_ID="$OPTARG"
      ;;
    h)
      display_help
      exit 0
      ;;
    b)
      BUILD_DEPS="*"
      ;;
    u)
      GENERATE_UPDATE_INFO=true
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
GENERATE_UPDATE_INFO=${GENERATE_UPDATE_INFO:-$DEFAULT_GENERATE_UPDATE_INFO}

# Checking if the folder really contains enea source folder
if ! check_path_validity "$SOURCE_DIR"; then
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
  SYSROOT_ARCH="undefined"
  PKGCONFIG_ARCH="x86_64-linux-gnu"
  APPIMAGE_ARCH="x86_64"
elif [ "$ENEA_ARCH" == "armv7hf" ]; then
  CONAN_ARCH="armv7hf"
  SKIP_TESTS="True"
  SYSROOT_ARCH="arm-buildroot-linux-gnueabihf"
  PKGCONFIG_ARCH="arm-linux-gnueabihf"
  APPIMAGE_ARCH="armhf"
elif [ "$ENEA_ARCH" == "aarch64" ]; then
  CONAN_ARCH="armv8"
  SKIP_TESTS="True"
  SYSROOT_ARCH="aarch64-buildroot-linux-gnu"
  PKGCONFIG_ARCH="aarch64-linux-gnu"
  APPIMAGE_ARCH="aarch64"
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

mkdir -p "$bundled_roms_dir"
if ! is_path_writable "$bundled_roms_dir"; then
  exit 1
fi

# Downloading the icon file
icon_url="https://web.enea.geniorio.it/resources/icon/icon.png"
icon_path="$downloads_dir/icon.png"
if ! download_file "$icon_url" "$icon_path"; then
    exit 1
fi

# Downloading bundled roms
bundled_roms_url="https://web.enea.geniorio.it/resources/bundled_roms"
if ! download_folder "$bundled_roms_url" "$bundled_roms_dir"; then
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

# If $DISPLAY is not set we assume there is no graphical environment so we go with direct rendering
if [ -z ${DISPLAY+x} ];
then
  ENEA_EXEC="enea_direct_rendering";
else
  ENEA_EXEC="enea";
fi

exec $APPDIR/usr/bin/$ENEA_EXEC' > "$launcher_path"

# Compiling enea
ENEA_TMP_BUILD_FOLDER="/tmp/enea_appimage_build"
rm -r $ENEA_TMP_BUILD_FOLDER
conan config install "$SOURCE_DIR/conan"

ENEA_BUILD_FOLDER="$ENEA_TMP_BUILD_FOLDER/build"
PKG_CONFIG_PATH="/usr/lib/$PKGCONFIG_ARCH/pkgconfig" \
  conan build -pr:h linux-$ENEA_ARCH-gcc-11.3-release -pr:b linux-x86_64-gcc-11.3-host --build "$BUILD_DEPS" -of $ENEA_BUILD_FOLDER \
  -o="use_direct_rendering=False" \
  -c tools.build:skip_test=$SKIP_TESTS "$SOURCE_DIR"

# Check if building enea was successful
if [ $? -ne 0 ]; then
    echo "Error: building enea failed."
    exit 1
fi

# Compiling enea with direct rendering support (we include both executables into AppImage and select the right one at runtime)
ENEA_DIRECT_RENDERING_BUILD_FOLDER="$ENEA_TMP_BUILD_FOLDER/build_direct_rendering"
PKG_CONFIG_PATH="/usr/lib/$PKGCONFIG_ARCH/pkgconfig" \
conan build -pr:h linux-$ENEA_ARCH-gcc-11.3-release -pr:b linux-x86_64-gcc-11.3-host --build "$BUILD_DEPS" -of $ENEA_DIRECT_RENDERING_BUILD_FOLDER \
  -o="use_direct_rendering=True" \
  -c tools.build:skip_test=$SKIP_TESTS "$SOURCE_DIR"

# Check if building direct rendering enea was successful
if [ $? -ne 0 ]; then
    echo "Error: building enea with direct rendering support failed."
    exit 1
fi

ENEA_EXEC="$ENEA_BUILD_FOLDER/build/Release/app/enea"

ENEA_DIRECT_RENDERING_EXEC="$ENEA_DIRECT_RENDERING_BUILD_FOLDER/build/Release/app/enea_direct_rendering"
mv "$ENEA_DIRECT_RENDERING_BUILD_FOLDER/build/Release/app/enea" "$ENEA_DIRECT_RENDERING_EXEC"

advmame_package_folder="$(conan cache path advmame/4.0:$(conan list -p "arch=$CONAN_ARCH" "advmame/4.0:*" | grep -A1 packages | grep -v packages | sed 's/^ *//'))"
ADVMAME_EXEC="$advmame_package_folder/bin/advmame"

# Downloading AppImage runtime for the correct architecture
runtime_url="https://github.com/AppImage/type2-runtime/releases/download/old/runtime-fuse2-$APPIMAGE_ARCH"
runtime_path="$downloads_dir/runtime-fuse2-$APPIMAGE_ARCH"
if ! download_file "$runtime_url" "$runtime_path"; then
    exit 1
fi

# If we are not packaging for x86_64 linuxdeploy cannot bundle dependencies. We do it manually instead
if [[ "$ENEA_ARCH" != "x86_64" ]]; then
  toolchain_package_folder="$(conan cache path toolchain-linux-$ENEA_ARCH-gcc-11.3/1.0:$(conan list "toolchain-linux-$ENEA_ARCH-gcc-11.3/1.0:*" | grep -A1 packages | grep -v packages | sed 's/^ *//'))"

  enea_dep="$(list_dependencies "$ENEA_EXEC" "$toolchain_package_folder/$SYSROOT_ARCH/sysroot")"
  advmame_dep="$(list_dependencies "$ADVMAME_EXEC" "$toolchain_package_folder/$SYSROOT_ARCH/sysroot")"
  dynamic_deps="$enea_dep $advmame_dep"
  export LDAI_RUNTIME_FILE="$runtime_path"
fi

# Using linuxdeploy to generate app dir
appimage_name="Enea-$ENEA_ARCH.AppImage"
appimage_output="$OUTPUT_DIR/$appimage_name"
echo "Generating "$appimage_output""
export LDAI_OUTPUT="$appimage_output"

# Generate update info only if requested
if [ "$GENERATE_UPDATE_INFO" = true ]; then
  export LDAI_UPDATE_INFORMATION="gh-releases-zsync|Slashcash|enea|latest|$appimage_name.zsync"
fi

# Sign AppImage if needed
if [ -n "$GPG_SIGNATURE_ID" ]; then
  export LDAI_SIGN=1
  export LDAI_SIGN_KEY="$GPG_SIGNATURE_ID"
fi

export NO_STRIP=1
export ARCH="$APPIMAGE_ARCH"

cd $OUTPUT_DIR
$deploy_path --appimage-extract-and-run --appdir="$app_dir" --custom-apprun="$launcher_path" --executable="$ENEA_EXEC" \
  --executable="$ENEA_DIRECT_RENDERING_EXEC" --executable="$ADVMAME_EXEC" $dynamic_deps --icon-file="$icon_path" --desktop-file="$desktop_path" --output appimage

# Check if linuxdeploy was successful
if [ $? -ne 0 ]; then
    echo "Error: linuxdeploy failed."
    exit 1
fi

exit 0
