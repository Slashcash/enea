#!/bin/bash

# Initialize variables with default values
executable_path=""
output_directory=""
env_file=""

# Set up required folders
base_temp_dir="/tmp/enea_appimage"
app_dir="$base_temp_dir/app"
downloads_dir="$base_temp_dir/downloads"

# Function to display usage help
function display_help {
  echo "Usage: $0 -e <executable_path>"
  echo "  -e  Specify the path of the enea executable"
  echo "  -o  Specify the output path"
  echo "  -t  Specify an optional environment file"
  echo "  -h  Display this help message"
  exit 1
}

# Function to check if a directory is writable
function is_writable {
  [ -w "$1" ]
}

# Function to clean up temporary folders
function cleanup {
  echo "Cleaning up temporary folders..."
  rm -rf "$base_temp_dir"
}

# Function to download a file
# Function to download a file
function download_file {
  local url="$1"
  local destination="$2"
  
  wget -q -O "$destination" "$url"
  
  # Check if the download was successful
  if [ $? -ne 0 ]; then
    echo "Error: Failed to download file from $url"
    exit 1
  else
    echo "File downloaded successfully: $destination"
  fi
}


# Trap function to ensure cleanup is called on exit
trap cleanup EXIT

# Parse command line options
while getopts "e:o:t:h" opt; do
  case "$opt" in
    e)
      executable_path="$OPTARG"
      ;;
    o)
      output_directory="$OPTARG"
      ;;
    t)
      env_file="$OPTARG"
      ;;
    h)
      display_help
      ;;
    \?)
      echo "Invalid option: -$OPTARG"
      display_help
      ;;
  esac
done

# Parsing environment file
set -o allexport
source ${env_file} set
+o allexport

# Check if linuxdeploy command exists
if ! command -v linuxdeploy &> /dev/null; then
  echo "Error: linuxdeploy command not found. Please install linuxdeploy."
  exit 1
fi

# Check if file command exists
if ! command -v file &> /dev/null; then
  echo "Error: file command not found. Please install file."
  exit 1
fi

# Check if wget command exists
if ! command -v wget &> /dev/null; then
  echo "Error: wget command not found. Please install curl."
  exit 1
fi

# Check if both executable path and output directory are provided
if [ -z "$executable_path" ] || [ -z "$output_directory" ]; then
  echo "Error: Please provide both the executable path with -e and the output directory with -o options."
  exit 1
fi

# Check if we have the ADVMAME_VERSION available
if [ -z ${ADVMAME_VERSION+x} ]; then 
  echo "Please set the ADVMAME_VERSION variable to a supported value"
  exit 1
fi

# Check if the executable exists
if [ -x "$executable_path" ]; then
  echo "Executable found: $executable_path"
else
  echo "Error: Executable not found at $executable_path"
  exit 1
fi

# Check the architecture of the executable
executable_arch=$(file "$executable_path" | awk '{print substr($7, 1, length($7)-1)}')
if [ "$executable_arch" == "x86-64" ]; then
  executable_arch="x86_64"
else
  echo "Error: We can't still generate appimage for other architectures: $executable_arch. Expected x86_64."
  exit 1
fi

# Check if the output directory is writable
if is_writable "$output_directory"; then
  echo "Output directory is writable: $output_directory"
else
  echo "Error: Output directory is not writable or does not exist: $output_directory"
  exit 1
fi

# Create the folders and check for errors
mkdir -p "$downloads_dir"
if [ $? -ne 0 ]; then
  echo "Error: Failed to create or folders already exist: $downloads_dir"
  exit 1
fi

mkdir -p "$app_dir"
if [ $? -ne 0 ]; then
  echo "Error: Failed to create or folders already exist: $app_dir"
  exit 1
fi

# Check if the base_temp_dir is writable
if is_writable "$base_temp_dir"; then
  echo "Base temporary directory is writable: $base_temp_dir"
else
  echo "Error: Base temporary directory is not writable or does not exist: $base_temp_dir"
  exit 1
fi

# Download the icon file
icon_url="https://enea.geniorio.it/icon/icon.png"
icon_path="$downloads_dir/icon.png"
download_file "$icon_url" "$icon_path"

# Download the advmame emulator
advmame_url="https://enea.geniorio.it/advmame/$ADVMAME_VERSION/$executable_arch/advmame"
advmame_path="$downloads_dir/advmame"
download_file "$advmame_url" "$advmame_path"

# Check if the download was successful
if [ $? -ne 0 ]; then
  echo "Error: Failed to download advmame file from $advmame_url"
  exit 1
else
  echo "Advmame file downloaded successfully: $advmame_path"
fi

# Create enea.desktop file
echo "[Desktop Entry]
Version=1.0
Type=Application
Icon=icon
Terminal=false
Exec=enea
Categories=Game;Emulator
Name=Enea" > "$base_temp_dir/enea.desktop"

# Create AppRun file
launcher_path="$base_temp_dir/enea_launcher.sh"
echo '#!/bin/bash
export PATH="$APPDIR/usr/bin" 
exec $APPDIR/usr/bin/enea' > "$launcher_path"

# Using linuxdeploy to generate app image
appimage_name="Enea-$executable_arch.AppImage"
export LDAI_OUTPUT="$output_directory/$appimage_name" 
export LDAI_UPDATE_INFORMATION="gh-releases-zsync|Slashcash|enea|latest|$appimage_name.zsync"
linuxdeploy --appimage-extract-and-run --appdir="$app_dir" --custom-apprun="$launcher_path" --executable="$executable_path" --executable="$advmame_path" --icon-file="$icon_path" --desktop-file="$base_temp_dir/enea.desktop" --output appimage --verbosity 2


# Check if linuxdeploy was successful
if [ $? -ne 0 ]; then
  echo "Error: linuxdeploy failed."
  exit 1
fi

exit 0