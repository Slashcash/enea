# Enea

Enea is a WIP AdvMAME frontend written in C++ for x86_64 Linux.

It currently looks like this:
![enea](https://i.ibb.co/LryqRHX/enea.png)

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

- The compilation toolchain can be set up using Conan by doing:
    
    `$ conan create -pr:h conan_profiles/enea-release-linux-x86_64-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 conan_recipes/toolchain-linux-x86_64`

- The [rocket](https://github.com/tripleslash/rocket) dependency is unavailable on official conan remote, we provide it via a self-written recipe:

    `$ conan create -pr:h conan_profiles/enea-release-linux-x86_64-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 -tf "" conan_recipes/rocket`

- The [SFML](https://www.sfml-dev.org/) dependency available on Conan remote is incompatible with our toolchain, we build a special version by doing:

    `$ conan create -pr:h conan_profiles/enea-release-linux-x86_64-11.3.0 -pr:b conan_profiles/enea-host-linux-x86_64-11.3.0 --build missing -o sfml*:network=False -o sfml*:audio=False conan_recipes/sfml`

- We can now install all the other needed dependencies:

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

From now on you can launch build commands as if building in a native environment (shouldn't be needed to install conan as it is already shipped within the container)
