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
            qt6.qtbase
            qt6.qtmultimedia
            qt6.qtwebengine

            SDL2
            boost
            libvlc
            ffmpeg
            vlc
            libvlcpp

            catch2
            clang-tools
            cmake
            gcc
            pkg-config
            pipewire
            gst_all_1.gstreamer
            gst_all_1.gst-plugins-good
            gst_all_1.gst-plugins-bad
            gst_all_1.gst-plugins-ugly
            gst_all_1.gst-plugins-base
            gst_all_1.gst-libav
          ];
        };
      };
    };
}