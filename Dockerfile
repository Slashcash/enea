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
    sudo

# Install conan
RUN pip install conan

# Add our own remote
RUN conan remote add --index 0 enea https://conan.geniorio.it

# Modify PKG_CONFIG_PATH, modifying it within conan recipes doesn't work (conan bug)
ENV PKG_CONFIG_PATH=/usr/lib/aarch64-linux-gnu/pkgconfig
