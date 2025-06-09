{
  lib,
  stdenv,
  fetchFromGitHub,
  cmake,
  pkg-config,
  qt6,
  libvlc,
  libvlcpp,
}:

stdenv.mkDerivation (finalAttrs: {
  pname = "mfaomp";
  version = "0.4.2";
#  libvlcppSrc = fetchFromGitHub {
#    owner = "videolan";
#    repo = "libvlcpp";
#    tag = "0.1.0";
#    hash = "sha256-nnS4DMz/2VciCrhOBGRb1+kDbxj+ZOnEtQmzs/TJ870=";
#  };

  src = fetchFromGitHub {
    owner = "Neurofibromin";
    repo = finalAttrs.pname;
    tag = "v${finalAttrs.version}";
    hash = "sha256-fNk7d3DfpVNmpI6t7BV0RO+ic9BPDlKtjAPN5ZP3P4E=";
  };

  nativeBuildInputs = [
    cmake
    pkg-config
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtmultimedia
    qt6.qtwebengine
    qt6.qtsvg
    libvlc
    libvlcpp
  ];

  cmakeFlags = [
    (lib.cmakeBool "USE_SYSTEM_PROVIDED_LIBVLCPP" true)
  ];

#  postPatch = ''
#    mkdir -p third_party/libvlcpp
#    cp -r ${finalAttrs.libvlcppSrc}/* ${finalAttrs.libvlcppSrc}/.??* third_party/libvlcpp/ 2>/dev/null || true
#  '';

  meta = {
    description = "Multiple Files At Once Media Player";
    homepage = "https://github.com/Neurofibromin/mfaomp";
    license = lib.licenses.gpl3Plus;
    maintainers = with lib.maintainers; [ neurofibromin ];
    platforms = lib.platforms.linux;
    mainProgram = "mfaomp";
  };
})