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

May work, but untested (and not packaged):

- MacOS
- BSD
- Ubuntu (24.04) (the toolchain setup is difficult)

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

| Windows | Linux | OSX (semi-supported) |
|---------|-------|----------------------|
|         |       |                      |
|         |       |                      |
|         |       |                      |

### Additional releases <a name="additional-releases"/>

Linux Installers: <br/>

| package   | x64 | arm64 |
|-----------|-----|-------|
| Flatpak	  |     |       |
| RPM	      |     |       |
| DEB	      |     |       |
| AppImage	 |     |       |

## Limitations <a name="limitations"/>

VLC does not support embedding in wayland windows, so the app always launches as an Xwindow. This will be fixed when VLC
adds support: https://code.videolan.org/videolan/vlc/-/issues/16106

Backends have differing feature-sets. QMediaPlayer supports rate change, but will not compensate the frequency shift in
audio.

## How it works <a name="how-it-works"/>

## Aims <a name="aims"/>

CI/CD

- [ ] [OpenSuse Build Service](https://build.opensuse.org/package/show/home:Neurofibromin/mfaomp)
- [x] [Nix](https://github.com/NixOS/nixpkgs/pull/414760)
- [ ] Flathub
- [ ] Debian
- [x] [Arch/AUR](https://aur.archlinux.org/packages/mfaomp)
- [x] [Fedora Copr](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/)
- [ ] winget
- [ ] chocolatey
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

Features:

- [x] QMediaPlayer backend
- [x] VLC backend
- [x] QtWebEngine backend
- [x] drag-and-drop
- [x] settings + menu + theming
- [x] testing setup
- [ ] plugin system for backends
- [x] compile flags for backends
- [ ] Chromium Embedded Framework (CEF) or
- [ ] FFmpeg backend
- [ ] SDL2? backend maybe something like this: https://github.com/fosterseth/sdl2_video_player
- [x] Use Strategy Pattern for handling backends (maybe)
- [ ] Use Visitor Pattern for handling backends (maybe)
- [x] Use Factory Pattern for handling backends (maybe)
- [ ] Add support for multiple backends at once
- [ ] Add support for subtitles
- [x] Adding videos via drag-and-drop
- [ ] Per video controls, overload right click menu
    - [ ] loop
    - [ ] skip
    - [ ] playback rate

### Roadmap

- [x] 0.4: qwebengine backend
- [x] 0.5: add Nix support
- [x] 0.5: drag-and-drop
- [x] 0.5: menu system with settings
- [x] 0.6: QTests
- [x] 0.6: theming, ui-ux design
- [x] 0.6: add Windows support
- [x] 0.6: add compile flags for enabling backends
- 0.6: add plugin system for runtime backend detection
- 0.7: sdl-ffmpeg backend
- 0.8: CEF backend
- 0.x: multithreaded approach
- 0.x: other graphics backends (opengl vulcan directx)
- 0.x: add c++ modules support
- 1.x: work on performance
- 1.x: self-contained build artifacts
- 1.x: reproducible build artifacts (SBOM)

Testing

- [ ] runtime dependencies
- [ ] integration tests
- [x] catch2

Bugs:

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
qt6-qtbase \
qt6-qtbase-common \
qt6-qtbase-devel \
qt6-qtbase-examples \
qt6-qtbase-gui \
qt6-qtbase-ibase \
qt6-qtbase-mysql \
qt6-qtbase-odbc \
qt6-qtbase-postgresql  \
qt6-qtbase-private-devel \
qt6-qtbase-static \
qt6-qtdeclarative-devel \
qt6-qtmultimedia-devel \
qt6-qttools-devel \
qt6-qtwebengine-devel \
boost-devel \
vlc-devel
```

### Arch:

### NixOS:

Use the provided flake.nix in project root:
```shell
nix develop
```

### Ubuntu:

```shell
sudo apt install \
build-essential \
cmake \
libboost-all-dev \
libqt6multimedia6 \
libqt6multimediawidgets6 \
libqt6webchannel6 \
libqt6webenginecore6 \
libqt6webenginewidgets6 \
libqt6webenginewidgets6 \
libqt6webview6 \
libvlc-dev \
pkg-config \
qt6-base-dev \
qt6-multimedia-dev \
qt6-tools-dev \
qt6-webengine-dev \
vlc-plugin-qt
```

### openSUSE:

```shell
sudo zypper in \
qt6-base-devel \
qt6-tools \
qt6-webview \
qt6-webengine \
qt6-multimediawidgets-devel \
qt6-multimedia-devel \
qt6-webenginewidgets-devel \
qt6-webchannel-devel \ 
vlc-devel
```

### Windows:

### Generic:

```
git clone -b master https://github.com/Neurofibromin/mfaomp mfaomp
cd mfaomp
mkdir build
cd build
cmake ..
make
make install
```