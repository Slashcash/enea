#!/bin/bash

# Default path (script's parent path)
DEFAULT_PATH="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"

# Sourcing common scripts
source $(dirname $0)/common.sh

# Check if clang-tidy and iwyu_tool are available
if ! is_command_available clang-tidy; then
  exit 1
fi

if ! is_command_available iwyu_tool.py; then
  exit 1
fi

# Parse command line options
while getopts ":s:" opt; do
  case ${opt} in
    s)
      SCAN_PATH=${OPTARG}
      ;;
    \?)
      echo "Usage: $(basename "$0") [-s <path>]"
      exit 1
      ;;
    :)
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      exit 1
      ;;
  esac
done

# If no path is provided, use the default path
SCAN_PATH=${SCAN_PATH:-$DEFAULT_PATH}

# Check if $SCAN_PATH looks like a valid source folder
if ! check_path_validity "$SCAN_PATH/.env"; then
  exit 1
fi

# Search for the compile command file
compile_commands=$(find "$SCAN_PATH" -type f -name "compile_commands.json")

# Count the number of files found
num_files=$(echo "$compile_commands" | wc -l)

# If there are multiple files found, exit with an error
if [ "$num_files" -gt 1 ]; then
  echo "Error: Multiple compile_commands.json files found."
  exit 1
fi

# If there's no file found, exit with an error
if [ "$num_files" -eq 0 ]; then
  echo "Error: No compile_commands.json file found."
  exit 1
fi

# Flag to track if any lint call fails
ERROR_FLAG=0

# Scan $SCAN_PATH/app, $SCAN_PATH/lib and $SCAN_PATH/test directories and clang-format files
tidy_directories=("$SCAN_PATH/app" "$SCAN_PATH/lib" "$SCAN_PATH/gui")
tidy_extensions="c cpp h hpp"
for dir in "${tidy_directories[@]}"; do
  for ext in $tidy_extensions; do
    while IFS= read -r -d '' file; do
      echo "Checking if $file is correctly linted"
      clang_tidy_command="clang-tidy -quiet -p $compile_commands $file"
      $clang_tidy_command > /dev/null 2>&1
      if [ $? -ne 0 ]; then
        ERROR_FLAG=1
        echo -e "\nError: $file is not correctly linted" 1>&2
        echo -e "Run \"$clang_tidy_command\" to have additional information\n" 1>&2
      fi

      iwyu_tool_command="iwyu_tool.py -p $compile_commands $file"
      $iwyu_tool_command > /dev/null 2>&1
      if [ $? -ne 0 ]; then
        ERROR_FLAG=1
        echo -e "\nError: $file does not have correct includes" 1>&2
        echo -e "Run \"$iwyu_tool_command\" to have additional information\n" 1>&2
      fi
    done < <(find "$dir" -type f -name "*.$ext" -not \( -name "gui.cpp" -o -name "inputmanager.cpp" \) -print0)
    #FIXME: excluding gui.cpp and inputmanager.cpp because clang-tidy 14 interacts badly with rocket.hpp (switching to a newer clang-tidy may help)
  done
done

# Exit with non-zero status if any call fails
if [ $ERROR_FLAG -ne 0 ]; then
  exit 1
fi
