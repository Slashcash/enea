# Ubuntu LTS version
FROM ubuntu:22.04

# Disable prompts during installation with apt-get
ARG DEBIAN_FRONTEND=noninteractive

# Add support to build for arm64 architecture
RUN dpkg --add-architecture arm64

RUN echo "deb [arch=arm64] http://ports.ubuntu.com/ jammy main restricted\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates main restricted\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy universe\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates universe\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy multiverse\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates multiverse\n\
deb [arch=arm64] http://ports.ubuntu.com/ jammy-backports main restricted universe multiverse" | tee /etc/apt/sources.list.d/arm64.list

ENV PKG_CONFIG_PATH="/usr/lib/aarch64-linux-gnu/pkgconfig"

RUN sed -i 's/^deb /deb [arch=amd64,i386] /' "/etc/apt/sources.list"

# Install apt dependencies
RUN apt update && apt install -y \
    python3-pip \
    git \
    pkg-config \
    wget \
    file \
    sudo

# Install conan
RUN pip install conan
