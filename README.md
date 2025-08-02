# mfaomp

Multiple Files At Once Media Player

## Contents

- [Status](#status)
- [Usage](#usage)
- [Installation & How to get](#installation--how-to-get)
    * [Fedora](#fedora)
    * [Arch](#arch)
    * [Standalone releases](#standalone-releases)
    * [Additional releases](#additional-releases)
- [Limitations](#limitations)
- [How it works](#how-it-works)
- [Releases](#releases)
    * [Aims](#aims)
    * [Signatures, hashes and integrity checks](#signatures-hashes-and-integrity-checks)
    * [Build instructions](#build-instructions)

## Status <a name="status"/>

[![Nightly](https://github.com/Neurofibromin/mfaomp/actions/workflows/nightly.yml/badge.svg)](https://github.com/Neurofibromin/mfaomp/actions/workflows/nightly.yml)

[![Build and Release](https://github.com/Neurofibromin/mfaomp/actions/workflows/build-release.yml/badge.svg)](https://github.com/Neurofibromin/mfaomp/actions/workflows/build-release.yml)

[![Copr build status](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/package/mfaomp/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/package/mfaomp/)

[![build result](https://build.opensuse.org/projects/home:Neurofibromin/packages/mfaomp/badge.svg?type=default)](https://build.opensuse.org/package/show/home:Neurofibromin/mfaomp)

[![Packaging status](https://repology.org/badge/vertical-allrepos/mfaomp.svg)](https://repology.org/project/mfaomp/versions)

## Usage <a name="usage"/>

## Installation & How to get <a name="installation--how-to-get"/>

Intended platforms:

- Arch
- openSUSE (Thumbleweed)
- Fedora (42)
- NixOS (25.05)
- Windows (10 22H2)
- Ubuntu (24.04)

Might work, but untested (and not packaged):

- MacOS
- BSD

### Fedora (COPR) <a name="fedora"/>

```shell
dnf copr enable neurofibromin/mfaomp
dnf install mfaomp
```

### Arch (AUR) <a name="arch"/>

```shell
yay mfaomp
```

### Standalone releases <a name="standalone-releases"/>

| Architecture  | Windows dynamic | Windows static | Linux dynamic | Linux static |
|---------------|-----------------|----------------|---------------|--------------|
| x86-32        |                 |                |               |              |
| x86-64/amd64  |                 |                |               |              |
| aarch64/arm64 |                 |                |               |              |

### Additional releases <a name="additional-releases"/>

Linux Installers: <br/>

| package   | x64 | arm64 |
|-----------|-----|-------|
| Flatpak	  |     |       |
| RPM	      |     |       |
| DEB	      |     |       |
| AppImage	 |     |       |

## Limitations <a name="limitations"/>

VLC does not support embedding in wayland windows, so the app always launches as an Xwindow. This can only be fixed when VLC
adds support: https://code.videolan.org/videolan/vlc/-/issues/16106

Backends have differing feature-sets. QMediaPlayer supports rate change, but will not compensate the frequency shift in
audio.

## How it works <a name="how-it-works"/>

Qt6 for the GUI which calls different media player backends.
At compile time not all backends have to be available, but only the found backends will be compiled. 
When the plugin system is implemented, the backends that were compiled need not be 
available at runtime, but loaded only if they are. 

## Aims <a name="aims"/>

### CI/CD

- [ ] [OpenSuse Build Service](https://build.opensuse.org/package/show/home:Neurofibromin/mfaomp)
- [x] [Nix](https://github.com/NixOS/nixpkgs/pull/414760)
- [ ] Flathub
- [ ] Debian/Ubuntu
- [x] [Arch/AUR](https://aur.archlinux.org/packages/mfaomp)
- [x] [Fedora Copr](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/)
- [ ] winget
- [ ] chocolatey
- [ ] appimage
- [x] separate nightly and release workflow
- [ ] Create Icon for the project
- [x] make backends optional dependencies, reflect this in the build files
- [x] version coherency with a script
- [ ] add gpg signatures and checksums to CICD pipelines
- [ ] install wizards
    - [ ] installshield
    - [ ] innosetup
    - [ ] nsis (only this works with CPack)
    - [ ] wix

### Features:

- [x] QMediaPlayer backend
- [x] VLC backend
- [x] QtWebEngine backend
- [x] drag-and-drop
- [x] settings + menu + theming
- [x] testing setup
- [x] compile flags for backends
- [ ] FFmpeg backend
- [ ] SDL2? backend maybe something like this: https://github.com/fosterseth/sdl2_video_player
- [x] Use Strategy Pattern for handling backends (maybe)
- [x] Use Visitor Pattern for handling backends (maybe)
- [x] Use Factory Pattern for handling backends (maybe)
- [x] Adding videos via drag-and-drop
- [x] No exceptions in the codebase
- [ ] Per video controls, overload right click menu
    - [ ] loop
    - [ ] playback rate
    - [ ] change backend on this video only

### Roadmap

- [x] 0.4: qwebengine backend
- [x] 0.5: add Nix support
- [x] 0.5: drag-and-drop
- [x] 0.5: menu system with settings
- [x] 0.6: QTests
- [x] 0.6: theming, ui-ux design
- [x] 0.6: add Windows support
- [x] 0.6: add compile flags for enabling backends
- [ ] 0.7: sdl backend
- [ ] 0.7: ffmpeg backend
- [ ] 0.8: swap backends to plugins with [dylib](https://github.com/martin-olivier/dylib) 
- [ ] 0.x: multithreaded approach
- [ ] 0.x: add c++ modules support
- [ ] 1.x: other graphics backends (opengl vulcan directx)
- [ ] 1.x: static builds
- [ ] 1.x: reproducible build artifacts (SBOM)

### Testing

Tests are meant to be run in debug mode (if NDEBUG is defined tests are meant to segfault).

- [ ] runtime dependencies
- [ ] integration tests
- [x] catch2

### Bugs:

### Signatures, hashes and integrity checks <a name="signatures-hashes-and-integrity-checks"/>

The following pgp keys are valid:

| Fingerprint                              | Description                                                 |
|------------------------------------------|-------------------------------------------------------------|
| 9F9BFE94618AD26667BD28214F671AFAD8D4428B | used in git and manually signed packages                    |
| 5C85EF4F34E089A04B2063A5833E01FC62E56779 | used in CICD pipelines to autosign packages where supported |

Keyservers:

- https://keys.openpgp.org/
- https://keyserver.ubuntu.com/
- http://pgp.mit.edu/

Public keys are also distributed with the build files.

SHA256 hashes for the binaries are produced in the GitHub workflow and found in checksum.txt under release assets.

## Build instructions <a name="build-instructions"/>

The following dependencies are a bit more than strictly necessary, probably. Only needed for building the project, not
for installation or using the program.

### Fedora:

```shell
sudo dnf install \
qt6-qtbase-devel \
qt6-qtmultimedia-devel \
qt6-qtwebengine-devel

sudo dnf install boost-devel 
sudo dnf install vlc vlc-devel      
sudo dnf install SDL2-devel

#for rpm fusion 
# sudo dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm
# sudo dnf install https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm
# sudo dnf install ffmpeg-devel --allowerasing
#without rpm fusion, not all codecs are suppported
sudo dnf install ffmpeg-free-devel 
```

### Arch:

```shell
sudo pacman -S base-devel cmake

sudo pacman -S qt6-base \
qt6-webengine \
qt6-multimedia

sudo pacman -S boost
sudo pacman -S vlc-plugins-all # https://wiki.archlinux.org/title/VLC_media_player#VLC_could_not_decode_the_format_%22[format]%22
sudo pacman -S ffmpeg
sudo pacman -S sdl2-compat
```

### Ubuntu:

```shell
sudo apt install build-essential cmake \
libboost-all-dev \
vlc \
libvlc-dev \
pkg-config \
ffmpeg \
libsdl2-dev

sudo apt install qt6-base-dev \
qt6-multimedia-dev \
qt6-webengine-dev

# as qtmediaplayer-ffmpeg is not available on ubuntu, the gstreamer backend has to be installed
sudo apt install gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-qt6 
```

### NixOS:

Use the provided flake.nix in project root:

```shell
nix develop
```



### openSUSE:

For some reason the vlc backend crashes the desktop environment on openSUSE. (only tested on KDE) 

```shell
sudo zypper install opi
opi codecs
sudo zypper in -t pattern devel_C_C++

sudo zypper in \
qt6-base-devel \
qt6-webengine \
qt6-multimediawidgets-devel \
qt6-multimedia-devel \
qt6-webenginewidgets-devel  

sudo zypper in \
sdl2-compat-devel \
ffmpeg \
ffmpeg-devel \
boost-devel \
libboost_system-devel \
libboost_filesystem-devel \
vlc-devel \
vlc
```

### Windows:

[//]: # (TODO: test this)
Install vlc and use vcpkg for the other dependencies.

### Generic:

```shell
git clone -b master https://github.com/Neurofibromin/mfaomp mfaomp
cd mfaomp
mkdir build
cd build
cmake ..
make
make install
```

Adjacent:

Example project of getting Qt and SDL to work together: [Qt-SDL](https://github.com/Neurofibromin/Qt-SDL)