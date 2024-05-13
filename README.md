[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
# Enea
*Enea* is an AdvanceMAME frontend for Linux. It wants to be a *launch-and-play* way to use the AdvanceMAME emulator. No setup needed: just download, click and play.

![enea](https://i.ibb.co/KmNmqRf/enea-v0-1-0.png)

## Getting Enea
You can either build Enea from source or download a pre-packaged binary from the project's [GitHub release page](https://github.com/Slashcash/enea/releases/latest).

You can then just launch the software by clicking on it or through a terminal, eg:

`$ ./Enea-x86_64.AppImage`

## Using
You can navigate the rom list using *Arrows* and then launch the rom by pressing *Enter*. You can exit Enea using the *Esc* button.

Roms and screenshots are searched in:

`~/.enea/roms`

You are supposed to fill this folder with roms and screenshot yourself. If no rom is found Enea will simply exit at startup. Be sure to provide at least a rom.

Please note that you will need an AdvanceMAME compatible romset for games to work.

Screenshots are supposed to be named using the very same rom pattern, eg:
if you want to provide a screenshot for *Street Fighter Alpa III* (rom name: *sfa3.zip*) it will need to be named *sfa3.png* (or any other suitable image extension).

## Building
The project ships with a Docker container which can be used to containerize the building process. Commands are meant to be launched from the project's root directory and assume a valid [Docker](https://www.docker.com/get-started/) installation is available within the system.

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Build from source and pack:

    `$ ./scripts/generate_appimage.sh`

    This script will build from source and pack *Enea* using the [AppImage](https://appimage.org/) format. You will find a file called *Enea-x86_64.AppImage* as a result in the project's source folder.

### Building without packing

While not recommended you may want to build *Enea* without packing it. In order for this executable to work you will need to launch it on a machine that provides a working AdvanceMAME installation. These commands can be launched in the very same Docker container used for building the .AppImage.

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Install additional dependency not provided by official Conan remote:

    `$ find conan_recipes -name "conanfile.py" -execdir conan export . \;`

- You can now start building the software by doing:

    `$ conan build -pr:h conan_profiles/linux-x86_64-gcc-11.3-release -pr:b conan_profiles/linux-x86_64-gcc-11.3-host -o build_tests=False --build "missing" .`

The compiled binary will now be available under `build/Release/enea`.
