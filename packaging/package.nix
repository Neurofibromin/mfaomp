{ pkgs ? import <nixpkgs> {} }:

let
  version = "0.4.0";
in
pkgs.stdenv.mkDerivation {
  pname = "mfaomp";
  inherit version;

  src = pkgs.lib.cleanSource ./.;

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    qt6.qtbase.dev
  ];

  buildInputs = with pkgs; [
    qt6.qtbase
    qt6.qtmultimedia
    qt6.qtsvg
    libvlc
  ];

  cmakeFlags = [
    "-DQT_QMAKE_EXECUTABLE=${pkgs.qt6.qtbase}/bin/qmake"
  ];

  meta = with pkgs.lib; {
    description = "Multiple Files At Once Media Player";
    homepage = "https://github.com/Neurofibromin/mfaomp";
    license = lib.licenses.gpl3Plus;
    maintainers = with lib.maintainers; [ neurofibromin ];
    platforms = [ "x86_64-linux" ];
    mainProgram = "mfaomp";
  };
}