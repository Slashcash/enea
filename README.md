![nightly workflow](https://github.com/Slashcash/enea/actions/workflows/build-nightly.yml/badge.svg?branch=main)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

# Enea
Enea is a *launch-and-play* solution to use the [AdvanceMAME](https://www.advancemame.it/) emulator on Linux. It is designed to turn your Linux machine into a MAME arcade emulator. Stop downloading, setupping and maintaining an AdvanceMAME installation: Enea will do it for you in less than a click.

![enea](https://i.ibb.co/KmNmqRf/enea-v0-1-0.png)

## Getting Enea
### Download
You can easily download an official release version of Enea from our [GitHub release page](https://github.com/Slashcash/enea/releases/latest). This is the easiest and suggested way of getting Enea, it should be enough for most users. Links are also conveniently provided in the table below:

<table style="border-collapse:collapse;border-spacing:0;margin:0px auto;table-layout: fixed; width: 828px" class="tg"><colgroup><col style="width: 41px"><col style="width: 166px"><col style="width: 621px"></colgroup>
<tbody>
<tr><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><img src="https://i.ibb.co/McGkxQR/linux-220x256.png" alt="Image" width="25" height="29"></td><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><a href="https://github.com/Slashcash/enea/releases/latest/download/Enea-x86_64.AppImage" target="_blank" rel="noopener noreferrer"><span style="font-weight:bold">Linux x86_64</span></a></td>
<td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><span style="font-weight:bold">Suitable for most Desktop Linux distribution</span></td></tr>
<tr><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><img src="https://i.ibb.co/tQfMWVv/raspberry-pi-200x256.png" alt="Image" width="25" height="29"></td><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><a href="https://github.com/Slashcash/enea/releases/latest/download/Enea-armv7hf.AppImage" target="_blank" rel="noopener noreferrer"><span style="font-weight:bold">Linux armv7hf</span></a></td>
<td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><span style="font-weight:bold">Suitable for every Raspberry Pi model (and every other armv7hf-based device)</span></td></tr>
<tr><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><img src="https://i.ibb.co/tQfMWVv/raspberry-pi-200x256.png" alt="Image" width="25" height="29"></td><td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><a href="https://github.com/Slashcash/enea/releases/latest/download/Enea-aarch64.AppImage" target="_blank" rel="noopener noreferrer"><span style="font-weight:bold">Linux aarch64</span></a></td>
<td style="border-color:inherit;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:middle;word-break:normal"><span style="font-weight:bold">Suitable (and suggested) for Raspberry Pi 3+ (and every other aarch64-based device)</span></td></tr>
</tbody></table>

### Download nightly build
You may want to download a freshly built version of Enea. In this case we also host some [nightly built binaries](https://github.com/Slashcash/enea/actions/workflows/build-nightly.yml). Unluckily due to a [known GitHub issue](https://github.com/actions/upload-artifact/issues/51) these builds are only accessible to registered GitHub users.

### Build from source
You can also build Enea from source (using a Linux-based machine). The project ships with a Docker container image which can be used to containerize the building process. For this reason the following compilation guide assumes that a working [Docker](https://www.docker.com/get-started/) installation is available within the system.

#### Build and pack
This is the easiest and suggested way to build Enea. It should be the go-to way for most users wishing to build the project. You will get a ready-to-be-launched executable packaged into a convenient [AppImage](https://appimage.org/) file.

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ ./scripts/launch_build_container.sh`

    You will be prompted for a password. This will be the root password for your Docker container. You will need this password for the build process.

- Choose the hardware architecture you want to build for:

    `$ export ENEA_ARCH="Linux-x86_64"`

    This will set the target architecture to `Linux-x86_64`. Allowed values are:
    - Linux-x86_64
    - Linux-aarch64
    - Linux-armv7hf

- Build from source and pack:

    `$ ./scripts/pack.sh -a ${ENEA_ARCH}`

    You will find a file called `Enea-${ENEA_ARCH}.AppImage` as a result in the project's source folder.

#### Build without packing
While not suggested you may want to build an unpacked version of Enea. This way you will get a pure executable (and not a packaged AppImage). This executable will probably not work straight away on most Linux distributions (you would need a working `advmame` installation and dynamic dependencies available within the executable environment).

- Build Docker container:

    `$ docker build --tag enea_build docker`

- Launch Docker container:

    `$ ./scripts/launch_build_container.sh`

    You will be prompted for a password. This will be the root password for your Docker container. You will need this password for the build process.

- Install custom conan configuration

    `$ conan config install conan`

- Choose the hardware architecture you want to build for:

    `$ export ENEA_ARCH="Linux-x86_64"`

    This will set the target architecture to `Linux-x86_64`. Allowed values are:
    - Linux-x86_64
    - Linux-aarch64
    - Linux-armv7hf

- You can now start building the software by doing:

    `$ conan build -pr:h linux-${ENEA_ARCH}-gcc-11.3-release -pr:b linux-x86_64-gcc-11.3-host --build "missing" -c tools.build:skip_test=True .`

The compiled binary will now be available under `build/Release/app` with the name of `enea`.

## Using Enea
Under most circumstances you will launch Enea through a terminal, eg:

`$ ./Enea-x86_64.AppImage`

On most Linux distributions (providing a desktop environments with direct AppImage support) you may also just click on the AppImage file itself.

### Providing roms

Enea works with the `MAME 0.106` romset, the romset is available on [torrent](https://tinyurl.com/mrun3vku). This torrent file is not provided, managed or maintained directly by us.

You should put the roms you wish to play under `~/.enea/roms`.

Should you fail to provide any rom Enea will start anyway, you will be able to play some self-provided public-domain roms.

### Providing screenshots
Enea is able to show rom screenshots to enhance user experience. You are supposed to put these screenshots under `~/.enea/roms`

Screenshots need to be named using the same name of the rom file they refer to, eg: if you want to provide a screenshot for Street Fighter Alpa III (rom name: `sfa3.zip`) it will need to be named `sfa3.png` (or any other suitable image extension).

### Input mapping
#### Rom selection
You can use the keyboard to navigate through the rom selection screen:

<table style="border-collapse:collapse;border-spacing:0;margin:0px auto" class="tg"><thead><tr><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">UP</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">DOWN</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Launch rom</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Exit</th></tr></thead>
<tbody><tr><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Arrow UP</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Arrow DOWN</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">1</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Esc</td></tr></tbody></table>

#### In game
You can use this keyboard mapping while playing a game:

<table style="border-collapse:collapse;border-spacing:0;margin:0px auto" class="tg"><thead>
<tr><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">UP</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">DOWN</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">LEFT</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">RIGHT</th>
<th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Action buttons</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Start</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Insert coin</th><th style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;font-weight:bold;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Exit<br></th></tr>
</thead>
<tbody>
<tr><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">w</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">s</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">a</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">d</td>
<td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">i, o, p, j, k, l</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">1</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">3</td><td style="border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;text-align:center;vertical-align:top;word-break:normal">Esc</td></tr>
</tbody></table>

### Joystick support
Enea currently provides limited external joystick/joypad support. You can use Enea with:

- Playstation 4 controller
- Mayflash V500 V2 joystick
