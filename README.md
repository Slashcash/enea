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
To build this software you will need a machine running a Linux distribution ([Ubuntu](https://ubuntu.com/download) is proven to work). This instructions also assume a working [Python](https://www.python.org/) environment. Every command is supposed to be launched from the project root folder.

The software is built using the [Conan](https://conan.io/) package manager. You can install it by doing:

`$ pip install conan`

## Generate AppImage
The preferred way to distribute *Enea* is through the [AppImage](https://appimage.org/) format. A convenient script that builds *Enea* and automatically packages into an .AppImage is provided:

`$ ./scripts/generate_appimage.sh`

This script will build from source and generate a file named `Enea-x86_64.AppImage` in the source folder

## Building native executable

While not recommended you may want to build *Enea* as a native executable. In order for this executable to work you will need to launch it on a machine that provides a working AdvanceMAME installation.

Since we use some custom Conan dependencies (unavailable on official remote) you will need to install them by doing:

`$ find conan_recipes -name "conanfile.py" -execdir conan export . \;`

You can now start building the software by doing:

`$ conan build -pr:h conan_profiles/linux-x86_64-gcc-11.3-release -pr:b conan_profiles/linux-x86_64-host -o build_tests=False --build "missing" .`

The compiled binary will now be available under `build/Release/enea`.
