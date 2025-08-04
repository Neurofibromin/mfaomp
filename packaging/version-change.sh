#!/bin/bash

# PKGBUILD
# ../README.md links
# mfaomp-copr.spec
# package.nix
# ../CMakeLists.txt
# ../vcpkg.json
# io.github.Neurofibromin.mfaomp.yaml

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <new_version>"
    exit 1
fi

NEW_VERSION=$1
NEW_DATE=$(date +'%Y. %m. %d')
PKGBUILD_FILE=./PKGBUILD
README_FILE=../README.md
SPEC_FILE1=./mfaomp-copr.spec
SPEC_FILE2=./mfaomp-obs.spec
NIX_FILE=./package.nix
CMAKELISTS_FILE=../CMakeLists.txt
VCPKG_FILE=../vcpkg.json
FLATPAK_YAML_FILE=./io.github.Neurofibromin.mfaomp.yaml

# Validate the PKGBUILD file exists
if [ ! -f "$PKGBUILD_FILE" ]; then
    echo "Error: File '$PKGBUILD_FILE' not found."
    exit 1
fi
# Replace version numbers (_tag, pkgver) in the PKGBUILD file
sed -i -E \
    -e "s/^_tag=[0-9]+\.[0-9]+\.[0-9]+/_tag=${NEW_VERSION}/" \
    -e "s/^pkgver=[0-9]+\.[0-9]+\.[0-9]+/pkgver=${NEW_VERSION}/" \
    "$PKGBUILD_FILE"
echo "Version updated to $NEW_VERSION in $PKGBUILD_FILE."

# Validate the README.md file exists
if [ ! -f "$README_FILE" ]; then
    echo "Error: File '$README_FILE' not found."
    exit 1
fi
# Replace all occurrences of the version in the file
sed -i -E "s|https://github.com/Neurofibromin/mfaomp/releases/download/v[0-9]+\.[0-9]+\.[0-9]+/mfaomp|https://github.com/Neurofibromin/mfaomp/releases/download/v${NEW_VERSION}/mfaomp|g" "$README_FILE"
sed -i -E "s/mfaomp-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp_[0-9]+\.[0-9]+\.[0-9]+/mfaomp_${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-linux-x86_64-v[0-9]+\.[0-9]+\.[0-9]+/mfaomp-linux-x86_64-v${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-flatpak-x86_64-v[0-9]+\.[0-9]+\.[0-9]+/mfaomp-flatpak-x86_64-v${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-x86_64-v[0-9]+\.[0-9]+\.[0-9]+/mfaomp-x86_64-v${NEW_VERSION}/g" "$README_FILE"
echo "Version updated to $NEW_VERSION in $README_FILE."

# Validate the copr spec file exists
if [ ! -f "$SPEC_FILE1" ]; then
    echo "Error: File '$SPEC_FILE1' not found."
    exit 1
fi
# Replace the Version value
sed -i -E "s/^Version:        [0-9]+\.[0-9]+\.[0-9]+/Version:        ${NEW_VERSION}/" "$SPEC_FILE1"
echo "Version updated to $NEW_VERSION in $SPEC_FILE1."

# Validate the obs spec file exists
if [ ! -f "$SPEC_FILE2" ]; then
    echo "Error: File '$SPEC_FILE2' not found."
    exit 1
fi
# Replace the Version value
sed -i -E "s/^Version:        [0-9]+\.[0-9]+\.[0-9]+/Version:        ${NEW_VERSION}/" "$SPEC_FILE2"
echo "Version updated to $NEW_VERSION in $SPEC_FILE2."

# Validate the nix file exists
if [ ! -f "$NIX_FILE" ]; then
    echo "Error: File '$NIX_FILE' not found."
    exit 1
fi
# Replace the Version value
sed -i -E "s/(version = \")[0-9]+\.[0-9]+\.[0-9]+(\";)/\1${NEW_VERSION}\2/" "$NIX_FILE"
#old: sed -i -E "s/^version = "[0-9]\+\.[0-9]\+\.[0-9]\+"/version = "${NEW_VERSION}/"" "$NIX_FILE"
echo "Version updated to $NEW_VERSION in $NIX_FILE."

# Validate the CMakeLists.txt file exists
if [ ! -f "$CMAKELISTS_FILE" ]; then
    echo "Error: File '$CMAKELISTS_FILE' not found."
    exit 1
fi
# Replace the VERSION value in the project command
sed -i -E "s/(VERSION )[0-9]+\.[0-9]+\.[0-9]+/\1${NEW_VERSION}/" "$CMAKELISTS_FILE"
echo "Version updated to $NEW_VERSION in $CMAKELISTS_FILE."

# Validate the vcpkg.json file exists
if [ ! -f "$VCPKG_FILE" ]; then
    echo "Error: File '$VCPKG_FILE' not found."
    exit 1
fi
# Replace the version in vcpkg.json
sed -i -E "s/\"version\": \"[0-9]+\.[0-9]+\.[0-9]+\"/\"version\": \"${NEW_VERSION}\"/" "$VCPKG_FILE"
echo "Version updated to $NEW_VERSION in $VCPKG_FILE."

# Validate the Flatpak YAML file exists
if [ ! -f "$FLATPAK_YAML_FILE" ]; then
    echo "Error: File '$FLATPAK_YAML_FILE' not found."
    exit 1
fi
# Replace the version in the URL line
sed -i -E "s|url: https://github.com/Neurofibromin/mfaomp/archive/refs/tags/v[0-9]+\.[0-9]+\.[0-9]+\.tar\.gz|url: https://github.com/Neurofibromin/mfaomp/archive/refs/tags/v${NEW_VERSION}.tar.gz|" "$FLATPAK_YAML_FILE"
echo "Version updated to $NEW_VERSION in $FLATPAK_YAML_FILE."