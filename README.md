# Enea

Enea is a WIP AdvMAME frontend written in C++ for x86_64 Linux.

It currently looks like this:
![enea](https://i.ibb.co/wWL9bWf/enea-v0-2-0.png)

I know it is not much. But it will improve!

You can navigate the rom list using Arrows and then launch the rom pressing Enter.

Rom are searched in:

`~/.enea/roms`

## Build
### Build in native environment
You will need a Linux distribution in order to build and launch the project. Build commands are meant to be launched from the project's root directory.

Toolchain and dependencies are managed using [Conan](https://conan.io/).

- Conan can be installed with pip:

    `$ pip install conan`

- We provide some recipes that are unavailable on official remote, let's add our own remote to the list:
    
    `$ conan remote add --index 0 https://conan.geniorio.it`

- We can now install all the needed dependencies:

    `$ conan install -pr:h conan_profiles/enea-release-linux-x86_64-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 --build missing -of build .`

- We are ready to configure the project by doing:

    `$ cd build && source conanbuild.sh && cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DENEA_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..`

- And then build it by doing:

    `$ cmake --build .`

- You can now launch *enea* by doing:

    `$ ./enea`

### Build in a docker container
The project ships with a Docker container which can be used to containerize the building process. Commands are meant to be launched from the project's root directory and assume a valid Docker installation is available within the system.

- Build Docker container 

`$ docker build --tag enea_build .`

- Launch Docker container

`$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

From now on you can launch build commands as if building in a native environment (shouldn't be needed to install conan and add the enea remote as they are already set within the container)
