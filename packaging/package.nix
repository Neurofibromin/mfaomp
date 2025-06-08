{
  lib,
  stdenv,
  fetchFromGitHub,
  cmake,
  pkg-config,
  qt6,
  libvlc,
}:

let
  version = "0.4.2";
  libvlcppSrc = fetchFromGitHub {
    owner = "videolan";
    repo = "libvlcpp";
    tag = "0.1.0";
    sha256 = "sha256-nnS4DMz/2VciCrhOBGRb1+kDbxj+ZOnEtQmzs/TJ870=";
  };
  pname = "mfaomp";
in
stdenv.mkDerivation {
  inherit version;
  inherit pname;

  src = fetchFromGitHub {
    owner = "Neurofibromin";
    repo = "mfaomp";
    tag = "v${version}";
    hash = "sha256-nh7prQzqPbRQUYzJUZ4cAILdn2ElRdEkiRXyJdrtf9A=";
  };

  nativeBuildInputs = [
    cmake
    pkg-config
#    qt6.qtbase.dev
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtmultimedia
    qt6.qtwebengine
    qt6.qtsvg
    libvlc
  ];

  cmakeFlags = [
#    "-DQT_QMAKE_EXECUTABLE=${qt6.qtbase}/bin/qmake"
#    "-DUSE_PREDOWNLOADED_LIBVLCPP=ON"
    (lib.cmakeBool "USE_PREDOWNLOADED_LIBVLCPP" true)
  ];

  postPatch = ''
    mkdir -p third_party/libvlcpp
    cp -r ${libvlcppSrc}/* ${libvlcppSrc}/.??* third_party/libvlcpp/ 2>/dev/null || true
  '';
#  installPhase = ''
#    runHook preInstall
#    cmake --install . --prefix=$out
#    runHook postInstall
#  '';

  meta = {
    description = "Multiple Files At Once Media Player";
    homepage = "https://github.com/Neurofibromin/mfaomp";
    license = lib.licenses.gpl3Plus;
    maintainers = with lib.maintainers; [ neurofibromin ];
    platforms = lib.platforms.linux;
    mainProgram = "mfaomp";
  };
}
