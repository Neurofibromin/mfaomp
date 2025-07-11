#!/bin/bash

# PKGBUILD
# ../README.md links
# mfaomp-copr.spec
# mfaomp-obs.spec
# package.nix
# ../CMakeLists.txt
# ../vcpkg.json

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
sed -i -E "s/mfaomp-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-${NEW_VERSION}/g" "$README_FILE"
# Also replace occurrences like "mfaomp_0.4.0" (underscores in certain links)
sed -i -E "s/mfaomp_[0-9]+\.[0-9]+\.[0-9]+/mfaomp_${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-win-x86-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-win-x86-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-win-arm64-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-win-arm64-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-linux-x64-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-linux-x64-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-linux-arm64-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-linux-arm64-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-osx-x64-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-osx-x64-${NEW_VERSION}/g" "$README_FILE"
sed -i -E "s/mfaomp-osx-arm64-[0-9]+\.[0-9]+\.[0-9]+/mfaomp-osx-arm64-${NEW_VERSION}/g" "$README_FILE"
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