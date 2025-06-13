# Cross compilation

## Fedora

```shell
#mkdir build
#cd build
mingw32-cmake -B build -S ../
mingw32-make -C build
```

requires the following packages:

- mingw32-qt6-qtbase
- mingw32-qt6-qtmultimedia
- mingw64-gcc-c++

## Arch

mingw-w64-gcc