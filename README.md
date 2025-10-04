# FlatOut 1 Chloe Collection

Companion plugin for my FlatOut 1 overhaul, [Chloe Collection](https://gaycoderprincess.github.io/project/chloe-collection-fo1).

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process.

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common), [nya-common-fo1](https://github.com/gaycoderprincess/nya-common-fo1), [nya-common-fouc](https://github.com/gaycoderprincess/nya-common-fouc), [FlatOutUCMenuLib](https://github.com/gaycoderprincess/FlatOutUCMenuLib), and [FlatOutTimeTrialGhosts](https://github.com/gaycoderprincess/FlatOutTimeTrialGhosts) to folders next to this one, so they can be found.

<br>

Required packages: `mingw-w64-gcc vcpkg`

### Arch
On arch, both packages can be installed through pacman
```console
sudo pacman -S mingw-w64-gcc vcpkg
```

vcpkg does not include the repo, so you'll have to follow it up with:
```console
git clone https://github.com/microsoft/vcpkg ~/.vcpkg/vcpkg
export VCPKG_ROOT="$HOME/.vcpkg/vcpkg"
```

### Debian
If you are on debian-based linux, it's a bit more complicated
```console
sudo apt-get install g++-mingw-w64-i686
```

apt does not have vcpkg so installing it will have to be done manually:
```console
git clone https://github.com/microsoft/vcpkg ~/.vcpkg/vcpkg
cd ~/.vcpkg/vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg integrate bash
export PATH="$PATH:$HOME/.vcpkg/vcpkg"
source ~/.bashrc
```

Which can be done all in one go with this monstrosity:
```console
git clone https://github.com/microsoft/vcpkg ~/.vcpkg/vcpkg && cd ~/.vcpkg/vcpkg && ./bootstrap-vcpkg.sh && ./vcpkg integrate install && ./vcpkg integrate bash && export PATH="$PATH:$HOME/.vcpkg/vcpkg" && source ~/.bashrc
```

<br>

If CLion does not auto-detect the compiler, go to ```Settings->Build, Execution, Deployment->Toolchains```

Set the C Compiler to ```i686-w64-mingw32-gcc```

and the C++ Compiler to ```i686-w64-mingw32-g++```

<br>



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
