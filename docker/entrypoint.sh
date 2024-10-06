#!/bin/bash

USER_ID=${LOCAL_USER_ID:-9001}

useradd --shell /bin/bash -u $USER_ID -o -c "" -m enea
usermod -a -G sudo enea

if [ -n "$LOCAL_USER_PASSWORD" ]; then
    printf "${LOCAL_USER_PASSWORD}\n${LOCAL_USER_PASSWORD}" | passwd -q enea
else
    echo "Please pass a LOCAL_USER_PASSWORD environment variable to container with your desired user password"
    exit 1
fi

if [ -n "$ENEA_WORK_FOLDER" ]; then
    	find ${ENEA_WORK_FOLDER} \! -user enea -exec chown enea '{}' +
else
    echo "Please pass a ENEA_WORK_FOLDER environment variable to container with a path on where to mount source folder within the container"
    exit 1
fi

exec gosu enea "$@"
