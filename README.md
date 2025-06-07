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

### Fedora (COPR) <a name="fedora"/>
```
dnf copr enable neurofibromin/mfaomp
dnf install mfaomp
```

### Arch (AUR) <a name="arch"/>
```shell
yay mfaomp
```

### Standalone releases <a name="standalone-releases"/>
| Windows        | Linux | OSX (semi-supported)        |
|----------------|-------|---|
| | | |
| | | |
| | | |

### Additional releases <a name="additional-releases"/>
Linux Installers: <br/>

| package      | x64                                                                                                                                                   | arm64                                                 |
|--------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------|
| Flatpak	    | | |
| RPM	        | | |
| DEB	        | | |
| AppImage	    | | |


## Limitations <a name="limitations"/>

VLC does not support embedding in wayland windows, so the app always launches as an Xwindow. This will be fixed when VLC adds support: https://code.videolan.org/videolan/vlc/-/issues/16106

Backends have differing feature-sets. QMediaPlayer supports rate change, but will not compensate the frequency shift in audio. 

## How it works <a name="how-it-works"/>

## Aims <a name="aims"/>
CI/CD
- [ ] [OpenSuse Build Service](https://build.opensuse.org/package/show/home:Neurofibromin/mfaomp)
- [ ] Nix
- [ ] Flathub
- [ ] Debian
- [x] [Arch/AUR](https://aur.archlinux.org/packages/mfaomp)
- [x] [Fedora Copr](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/)
- [ ] winget
- [ ] chocolatey
- [x] separate nighly and release workflow
- [ ] Create Icon for the project
- [ ] make backends optional dependencies, reflect this in the build files
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
- [ ] Chromium Embedded Framework (CEF) or
- [ ] FFmpeg backend
- [ ] SDL2? backend maybe something like this: https://github.com/fosterseth/sdl2_video_player
- [ ] Use Strategy Pattern for handling backends (maybe)
- [ ] Use Visitor Pattern for handling backends (maybe)
- [ ] Add support for multiple backends at once
- [ ] Add support for subtitles
- [x] Adding videos via drag-and-drop
- [ ] Per video controls, overload right click menu
  - [ ] loop 
  - [ ] skip 
  - [ ] playback rate 

### Roadmap

- 0.4: qwebengine backend
- 0.4: add Windows support
- 0.5: add Nix support
- 0.5: menu system with settings
- 0.5: drag-and-drop
- 0.6: CEF backend
- 0.6: theming, ui-ux design
- 0.7: sdl-ffmpeg backend
- 0.x: multithreaded approach
- 0.x: other graphics backends (opengl vulcan directx)
- 0.x: add c++ modules support
- 1.x: work on performance
- 1.x: self contained build artifacts
- 1.x: reproducible build artifacts (SBOM)

Testing
- [ ] testing packaging, esp. runtime dependencies

Bugs:
- [ ] QTWebEngine not compiled with proprietary codecs => no playing some codecs
- [x] QTWebEngine not initialized before play() is called, so autoplay fails
- [x] QTWebEngine -> something else time get doesn't work
- [x] something else -> QTWebEngine time set doesn't work

### Signatures, hashes and integrity checks <a name="signatures-hashes-and-integrity-checks"/>

The following pgp keys are valid:

| Fingerprint | Description |
| ----------- | ----------- |
| 9F9BFE94618AD26667BD28214F671AFAD8D4428B | used in git and manually signed packages |
| 5C85EF4F34E089A04B2063A5833E01FC62E56779 | used in CICD pipelines to autosign packages where supported |

Keyservers:
- https://keys.openpgp.org/
- https://keyserver.ubuntu.com/
- http://pgp.mit.edu/

Public keys are also distributed with the build files.

SHA256 hashes for the binaries are produced in the GitHub workflow and found in checksum.txt under release assets.

## Build instructions <a name="build-instructions"/>
Install dependencies:
- vlc
- qmedia?

```
git clone -b master https://github.com/Neurofibromin/mfaomp mfaomp
cd mfaomp
mkdir build
cd build
cmake ..
make
make install
```