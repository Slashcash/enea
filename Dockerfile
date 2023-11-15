# Ubuntu LTS version
FROM ubuntu:22.04

# Disable prompts during installation with apt-get
ARG DEBIAN_FRONTEND=noninteractive

# Install apt dependencies
RUN apt update && apt install -y \
    python3-pip \
    git \
    pkg-config \
    sudo

# Install conan
RUN pip install conan

# Add our own remote
RUN conan remote add --index 0 enea https://conan.geniorio.it