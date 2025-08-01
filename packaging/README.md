# Commands to test the package descriptor files

First install all build dependencies required as described in the root README.

## Arch AUR

sudo pacman -S pacman-contrib namcap
updpkgsums PKGBUILD
makepkg --syncdeps PKGBUILD
namcap --info PKGBUILD
namcap --info ./mfaomp-0.7.1-1-x86_64.pkg.tar.zst

## Fedora COPR

sudo dnf install fedora-packager rpmdevtools
rpmdev-setuptree
spectool -g -R mfaomp-copr.spec #fetch sources
sudo dnf builddep mfaomp-copr.spec #install all the required dependencies
rpmbuild -ba mfaomp-copr.spec #build the spec file

===============================================

## Nix

git clone --depth 1 https://github.com/neurofibromin/nixpkgs.git
git remote add upstream https://github.com/NixOS/nixpkgs.git
git fetch upstream
git checkout -b upstream-master upstream/master
git pull
nix-build mfaomp



## Flatpak
docs: https://docs.flatpak.org/en/latest/index.html
flatpak remote-add --user --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak-builder build --user --install-deps-from=flathub --force-clean --ccache --install org.kde.kate.json
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo --install builddir org.flatpak.Hello.yml
flatpak run org.kde.kat
flatpak build-bundle repo hello.flatpak org.flatpak.Hello --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
## Ubuntu PPA
## Appimage
## Opensuse OBS
