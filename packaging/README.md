# Commands to test the package descriptor files

First install all build dependencies required as described in the root README.

## Arch AUR

sudo pacman -S pacman-contrib namcap
updpkgsums PKGBUILD
makepkg --syncdeps PKGBUILD
namcap --info PKGBUILD
namcap --info ./mfaomp-0.7.1-1-x86_64.pkg.tar.zst
makepkg --printsrcinfo > .SRCINFO

in aur git repo:

git add PKGBUILD .SRCINFO
git commit -m "new version"
git push

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
touch nixpkgs/pkgs/by-name/mf/mfaomp/package.nix
nix-build mfaomp



## Flatpak

docs: https://docs.flatpak.org/en/latest/index.html

```shell
sudo dnf install flatpak flatpak-builder
flatpak remote-add --user --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak-builder build-dir --force-clean --user --install-deps-from=flathub --repo=repo --ccache --install io.github.Neurofibromin.mfaomp.yaml
flatpak run io.github.Neurofibromin.mfaomp
flatpak build-bundle repo mfaomp.flatpak io.github.Neurofibromin.mfaomp --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
```

## Ubuntu PPA

## Appimage

## Opensuse OBS
docs:
- https://en.opensuse.org/openSUSE:OSC#Building_packages
- https://en.opensuse.org/openSUSE:Build_Service_Tutorial
- https://documentation.suse.com/en-us/sbp/systems-management/html/SBP-RPM-Packaging/index.html

```shell
sudo zypper in osc # rpmdevtools rpm-build # not needed 
osc checkout home:Neurofibromin
cd home\:Neurofibromin/mfaomp
#put here: spec file + changes file + _service file 
# spec-cleaner -i mfaomp.spec
#added _service file so no longer needed: osc service runall download_files
osc build --clean
osc vc #add changelog, CAREFUL, this by default publishes in the .changes the email address set in OBS
osc add mfaomp.spec mfaomp.changes _service
osc ci -m "new version"
```
