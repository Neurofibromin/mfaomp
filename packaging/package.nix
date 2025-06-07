{ pkgs ? import <nixpkgs> {} }:

let
  version = "0.4.1";
in
pkgs.stdenv.mkDerivation {
  pname = "mfaomp";
  inherit version;

#  src = pkgs.lib.cleanSource ./.;

  src = pkgs.fetchFromGitHub {
    owner = "Neurofibromin";
    repo = "mfaomp";
    rev = "v${version}";
    sha256 = "sha256-123=";
  };

  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    qt6.qtbase.dev
    qt6.wrapQtAppsHook
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


  installPhase = ''
    runHook preInstall
    cmake --install . --prefix=$out
    runHook postInstall
  '';

  postInstall = ''
    install -Dm644 LICENSE $out/share/doc/${pname}/LICENSE.txt
  '';

  meta = with pkgs.lib; {
      description = "Multiple Files At Once Media Player";
      homepage = "https://github.com/Neurofibromin/mfaomp";
      license = lib.licenses.gpl3Plus;
      maintainers = with lib.maintainers; [ neurofibromin ];
      platforms = [ "x86_64-linux" ];
      mainProgram = "mfaomp";
    };
}