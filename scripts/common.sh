# Function to check if the specified path is a valid readable directory
is_path_readable() {
    local path="$1"
    if [ ! -d "$path" ] || [ ! -r "$path" ]; then
        echo "Error: $path is not a valid readable directory" 1>&2
        return 1
    else
        return 0
    fi
}

is_path_writable() {
    local path="$1"
    if [ -w "$path" ]; then
        return 0;
    else
        echo "Error: $path is not a valid writable directory" 1>&2
        return 1
    fi
}

# Function to check if we are running inside the source folder (assuming it's valid if we find our .enea file)
check_path_validity() {
    local env_file="$1/.enea"
    local path=$(dirname "$env_file")
    local inspect_env="grep -q \"^SOFTWARE_NAME=Enea$\" \"$env_file\""
    if [ "is_path_readable \"$path\"" ] && [ -f "$env_file" ] && [ "$inspect_env" ]; then
        return 0
    else
        echo "Error: $1 does not seem to be a source folder or it is not readable" 1>&2
        return 1
    fi
}

# Function to check if a command is available
is_command_available() {
    local cmd="$1"
    if ! command -v "$cmd" &> /dev/null; then
        echo "Error: $cmd command not found. Please install $cmd."
        return 1
    else
        return 0
    fi
}

# Function to download a file
function download_file {
    local url="$1"
    local destination="$2"

    # Check if wget is available
    if ! is_command_available wget; then
        retun 1
    fi

    wget -q -O "$destination" "$url"

    # Check if the download was successful
    if [ $? -ne 0 ]; then
        echo "Error: Failed to download file from $url"
        return 1
    fi
}
