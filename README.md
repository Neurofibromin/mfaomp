# mfaomp
Multiple Files At Once Media Player

## Contents

- [Status](#status)
- [Usage](#usage)
- [Installation & How to get](#installation--how-to-get)
    * [Standalone releases](#standalone-releases)
    * [Additional releases](#additional-releases)
- [Limitations](#limitations)
- [How it works](#how-it-works)
- [Releases](#releases)
    * [Aims](#aims)
    * [Signatures, hashes and integrity checks](#signatures-hashes-and-integrity-checks)
    * [Build instructions](#build-instructions)


## Status <a name="status"/>

## Usage <a name="usage"/>

## Installation & How to get <a name="installation--how-to-get"/>

### Standalone releases <a name="standalone-releases"/>
| Windows                                                                                                          | Linux                                                                                                          | OSX (semi-supported)                                                                                     |
|------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|
| [x64]()     | | |
| [x32]()     | | |
| [arm64]()   | | |

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

## Releases <a name="releases"/>

[Standalone releases](#standalone-releases)

[Additional releases](#additional-releases)

### Aims <a name="aims"/>
CI/CD
- [ ] [OpenSuse Build Service](https://build.opensuse.org/package/show/home:Neurofibromin/mfaomp)
- [ ] Nix
- [ ] Flathub
- [ ] Debian
- [ ] [Arch/AUR](https://aur.archlinux.org/packages/mfaomp)
- [ ] [Fedora Copr](https://copr.fedorainfracloud.org/coprs/neurofibromin/mfaomp/)
- [ ] innosetup
- [ ] winget
- [ ] chocolatey

Features:
- [ ] SDL2? backend
- [ ] FFmpeg backend
- [ ] Chromium Embedded Framework (CEF) or QtWebEngine backend
- [ ] Use Strategy Pattern for handling backends (maybe)

Testing
- [ ] testing workflows
- [ ] testing packaging

Bugs:
- [ ] 

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

### Build instructions <a name="build-instructions"/>
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