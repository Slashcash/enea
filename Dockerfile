# Ubuntu LTS version
FROM ubuntu:22.04

# Disable prompts during installation with apt-get
ARG DEBIAN_FRONTEND=noninteractive

# Add arm64 architecture
RUN dpkg --add-architecture arm64

## arch-qualify the current repositories
RUN sed -i "s/deb h/deb [arch=amd64] h/g" /etc/apt/sources.list

## add arm64's repos
RUN echo "# arm64 repositories" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy main restricted" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-updates main restricted" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy universe" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-updates universe" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy multiverse" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-updates multiverse" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-backports main restricted universe multiverse" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-security main restricted" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-security universe" >> /etc/apt/sources.list
RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-security multiverse" >> /etc/apt/sources.list

# Install apt dependencies
RUN apt update && apt install -y \
    python3-pip \
    git \
    pkg-config \
    wget \
    file \
    sudo

# These dependencies are only needed in order to pack advmame within the AppImage
RUN apt install -y \
    libsdl2-dev \
    libasound2-dev

# Install conan
RUN pip install conan 

# Install python-dotenv (used in our conanfile.py)
RUN pip install python-dotenv

# Add our own remote
RUN conan remote add --index 0 enea https://conan.geniorio.it

# Modify PKG_CONFIG_PATH, modifying it within conan recipes doesn't work (conan bug)
ENV PKG_CONFIG_PATH=/usr/lib/aarch64-linux-gnu/pkgconfig

# Install linuxdeploy (needed to create AppImage)
RUN wget -O /usr/bin/linuxdeploy https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-static-x86_64.AppImage && chmod +x /usr/bin/linuxdeploy
