## v0.7.0 (2024-11-16)

### Feat

- sign release artifacts with GPG

### Fix

- stop crashing randomly when connecting a joystick

### Refactor

- add a separate singleton class
- rewrite database class for better log experience

## v0.6.0 (2024-11-10)

### Feat

- upgrade advmame to version 4.0

## v0.5.0 (2024-11-10)

### Feat

- add native support for Mayflash F500 V2

### Refactor

- do not use a fixed mapping for inputs
- improve information retrieval from internal databases
- add requirements for templates on some utility functions

## v0.4.0 (2024-10-26)

### Feat

- add support for ps4 controller

### Refactor

- improve RomMedia class
- improve RomInfo class

## v0.3.1 (2024-10-19)

### Fix

- stop exiting if no rom is found in search folder

## v0.3.0 (2024-10-13)

### Feat

- show build author in bottom left corner of the gui
- show git commit of build in bottom left corner of gui

### Fix

- enable framebuffer when launching a game

## v0.2.2 (2024-10-05)

### Fix

- do not list bios as launchable roms
- solve crash at startup on arm architecture

### Refactor

- manage rom source cache management within RomSource class
- reimplement romdb class as a singleton
- reimplement configuration class as a singleton

## v0.2.1 (2024-06-22)

### Fix

- set frame rate limit in order to reduce cpu usage

### Refactor

- use json format instead of xml format for internal rom database
- rewrite Rom class and test suite
- create separate RomMedia struct
- move RomInfo struct outside of RomDB class

## v0.2.0 (2024-06-17)

### Feat

- implement support for player 2

## v0.1.0 (2024-05-12)

### Feat

- first implementation of enea
