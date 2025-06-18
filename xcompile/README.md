# Cross compilation

## Fedora

```shell
#mkdir build
#cd build
mingw32-cmake -B build -S ../
mingw32-make -C build
```

Requires the following packages:

- mingw32-qt6-qtbase
- mingw32-qt6-qtmultimedia
- mingw32-gcc-c++
- mingw64-gcc-c++
- mingw32-gcc
- mingw64-gcc
- mingw-w64-tools

## Arch

mingw-w64-gcc