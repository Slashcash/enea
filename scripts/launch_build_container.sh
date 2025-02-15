#!/bin/bash
DEFAULT_SOURCE_DIR="$( cd "$(dirname "$0/..")" >/dev/null 2>&1 ; pwd -P )"

# Checking if the folder really contains enea source folder
source $(dirname $0)/common.sh
if ! check_path_validity $DEFAULT_SOURCE_DIR; then
  exit 1
fi

read -s -p "Define an user password for your docker container: " password

DOCKER_USER_ID=$(id -u) DOCKER_USER_PASSWORD=$password docker compose -f docker/docker-compose.yaml run --rm enea_build
