![example workflow](https://github.com/Slashcash/enea/actions/workflows/build.yml/badge.svg?event=schedule;branch=main)

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
# Enea
*Enea* is an AdvanceMAME frontend for Linux. It wants to be a *launch-and-play* way to use the AdvanceMAME emulator. No setup needed: just download, click and play.

![enea](https://i.ibb.co/KmNmqRf/enea-v0-1-0.png)

## Getting Enea
You can either build Enea from source or download a pre-packaged binary from the project's [GitHub release page](https://github.com/Slashcash/enea/releases/latest).

You can then just launch the software by clicking on it or through a terminal, eg:

`$ ./Enea-x86_64.AppImage`

### Nightly builds
We also host some [nightly built binaries](https://github.com/Slashcash/enea/actions/workflows/build.yml?query=event%3Aschedule). They can be useful if you need a recently built binary of *Enea*.

## Using
Roms and screenshots are searched in:

`~/.enea/roms`

You are supposed to fill this folder with roms and screenshot yourself. **If no rom is found Enea will simply exit at startup.** Be sure to provide at least a rom.

Control mapping as follows:

*Frontend*
---
| UP       | DOWN       | Launch rom | Exit |
|----------|------------|------------|------|
| Arrow Up | Arrow Down | Enter      | Esc  |

*In game (Player 1)*
---
|   P1 UP  |   P1 DOWN  |   P1 LEFT  |   P1 RIGHT  |       Action buttons      | P1 START | P1 Insert Coin | Exit Game |
|:--------:|:----------:|:----------:|:-----------:|:-------------------------:|:--------:|:--------------:|:---------:|
| Arrow Up | Arrow Down | Arrow Left | Arrow Right | 7, 8, 9, 4, 5, 6 (keypad) |     1    |        3       |    Esc    |

*In game (Player 2)*
---
| P2 UP | P2 DOWN | P2 LEFT | P2 RIGHT |  Action buttons  | P2 START | P2 Insert Coin |
|:-----:|:-------:|:-------:|:--------:|:----------------:|:--------:|:--------------:|
|   w   |    s    |    a    |     d    | i, o, p, j, k, l |     2    |        4       |

Please note that you will need an AdvanceMAME compatible romset for games to work.

Screenshots are supposed to be named using the very same rom pattern, eg:
if you want to provide a screenshot for *Street Fighter Alpa III* (rom name: *sfa3.zip*) it will need to be named *sfa3.png* (or any other suitable image extension).

## Building
The project ships with a Docker container which can be used to containerize the building process. Commands are meant to be launched from the project's root directory and assume a valid [Docker](https://www.docker.com/get-started/) installation is available within the system.

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Choose the hardware architecture you want to build for:

    `$ export ENEA_ARCH="x86_64"`

    Allowed values are:
    - x86_64
    - aarch64
    - armv7hf

- Build from source and pack:

    `$ ./scripts/generate_appimage.sh -a ${ENEA_ARCH}`

    This script will build from source and pack *Enea* using the [AppImage](https://appimage.org/) format. You will find a file called *Enea-${ENEA_ARCH}.AppImage* as a result in the project's source folder.

### Building without packing

While not recommended you may want to build *Enea* without packing it. In order for this executable to work you will need to launch it on a machine that provides a working AdvanceMAME installation. These commands can be launched in the very same Docker container used for building the .AppImage.

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ docker run --rm -v .:/enea -w /enea -ti enea_build /bin/bash`

- Install custom conan configuration

    `$ conan config install conan`

- Choose the hardware architecture you want to build for:

    `$ export ENEA_ARCH="x86_64"`

    Allowed values are:
    - x86_64
    - aarch64
    - armv7hf

- You can now start building the software by doing:

    `$ conan build -pr:h linux-${ENEA_ARCH}-gcc-11.3-release -pr:b linux-x86_64-gcc-11.3-host --build "missing" -c tools.build:skip_test=True .`

The compiled binary will now be available under `build/Release/app/enea`.
