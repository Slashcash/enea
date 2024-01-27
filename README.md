# Enea

Enea is a WIP AdvMAME frontend written in C++ for Linux.

It currently looks like this:
![enea](https://i.ibb.co/wWL9bWf/enea-v0-2-0.png)

I know it is not much. But it will improve!

You can navigate the rom list using Arrows and then launch the rom pressing Enter.

Rom are searched in:

`~/.enea/roms`

## Build
The project ships with a Docker container which can be used to containerize the building process. Commands are meant to be launched from the project's root directory and assume a valid Docker installation is available within the system.

- Build Docker container 

    `$ docker build --tag enea_build .`

- Launch Docker container

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Choose the target architecture by doing

    `$ export ENEA_ARCH=x86_64`

    You can modify ENEA_ARCH variable at the start of the build command in order to build for different architectures. Right now supported architectures are:
    - x86_64
    - aarch64

- We are ready to build the project by doing:

    `$ conan build -pr:h conan_profiles/enea-release-linux-${ENEA_ARCH}-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 -o enea*:build_tests=False --build "*" -of build-${ENEA_ARCH} .`


