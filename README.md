# Enea

Enea is a WIP AdvMAME frontend written in C++ for Linux.

It currently looks like this:
![enea](https://i.ibb.co/wWL9bWf/enea-v0-2-0.png)

I know it is not much. But it will improve!

## Launching
You can either build Enea from source or download a pre-packaged binary from the project's [GitHub release page](https://github.com/Slashcash/enea/releases/latest).

You can then just launch the software by clicking on it or through a terminal, eg:

`$ ./Enea-x86_64.AppImage`

## Using
You can navigate the rom list using Arrows and then launch the rom pressing Enter.

Rom are searched in:

`~/.enea/roms`

## Building
The project ships with a Docker container which can be used to containerize the building process. Commands are meant to be launched from the project's root directory and assume a valid Docker installation is available within the system.

- Build Docker container 

    `$ docker build --tag enea_build .`

- Launch Docker container

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Choose the target architecture by doing

    `$ export ENEA_ARCH=x86_64`

    You can choose ENEA_ARCH to build for different architectures. Right now supported architectures are:
    - x86_64
    - aarch64

- We are ready to build the project by doing:

    `$ conan build -pr:h conan_profiles/enea-release-linux-${ENEA_ARCH}-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 -o enea*:build_tests=False --build "*" -of build-${ENEA_ARCH} .`

- Even if Enea can be theoretically launched stand-alone the preferred distribution format is [AppImage](https://appimage.org/). You can generate an AppImage for Enea by doing:

    `$ ./scripts/generate_appimage.sh -e build-${ENEA_ARCH}/build/Release/enea -o build-${ENEA_ARCH} -t .env`