# FlatOut 1 Chloe Collection

Companion plugin for my FlatOut 1 modpack, [Chloe Collection](https://gaycoderprincess.github.io/project/chloe-collection-fo1).

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process.

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) and [nya-common-fo1](https://github.com/gaycoderprincess/nya-common-fo1) to folders next to this one, so they can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x86-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib32`

You should be able to build the project now in CLion.
