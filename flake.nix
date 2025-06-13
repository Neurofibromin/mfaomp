{
  description = "mfaomp dev env";
  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };
  outputs = inputs@{ flake-parts, nixpkgs, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
      ];
      perSystem = { config, self', inputs', pkgs, system, ... }: {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            boost
            catch2
            cmake
            pkg-config
            gcc
            clang-tools
            libvlc
            libvlcpp
            qt6.qtbase
            qt6.qtmultimedia
          ];
        };
      };
    };
}