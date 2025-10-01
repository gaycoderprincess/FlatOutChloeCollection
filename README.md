# FlatOut 1 Chloe Collection

Companion plugin for my FlatOut 1 overhaul, [Chloe Collection](https://gaycoderprincess.github.io/project/chloe-collection-fo1).

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process.

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common), [nya-common-fo1](https://github.com/gaycoderprincess/nya-common-fo1), [nya-common-fouc](https://github.com/gaycoderprincess/nya-common-fouc), [FlatOutUCMenuLib](https://github.com/gaycoderprincess/FlatOutUCMenuLib), and [FlatOutTimeTrialGhosts](https://github.com/gaycoderprincess/FlatOutTimeTrialGhosts) to folders next to this one, so they can be found.

If you haven't used CLion before, you'll need to install the mingw g++ compiler, that can be done in the terminal with:
```console
sudo pacman -S mingw-w64-gcc
```

If you are on debian-based linux, it's this command
```console
sudo apt-get install g++-mingw-w64-i686
```

<br>

If CLion does not auto-detect the compiler, go to ```Settings->Build, Execution, Deployment->Toolchains```

Set the C Compiler to ```i686-w64-mingw32-gcc```

and the C++ Compiler to ```i686-w64-mingw32-g++```

<br>

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

<br>

To install the BASS audio library:

Download the Win32 version from [here](https://www.un4seen.com/bass.html) and extract it somewhere

Once that's done, copy `bass.lib` from the `c` folder into `nya-common/lib32`

<br>

You should be able to build the project now in CLion.
