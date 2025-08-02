%global debug_package %{nil}
Name:           mfaomp
Version:        0.7.3
Release:        %autorelease
Summary:        Multiple Files At Once Media Player

License:        GPL-3.0-or-later
URL:            https://github.com/Neurofibromin/mfaomp
Source0:        %{url}/archive/refs/tags/v%{version}.tar.gz
Source1:        https://github.com/videolan/libvlcpp/archive/refs/heads/master.tar.gz#/libvlcpp-master.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtmultimedia-devel
BuildRequires:  qt6-qtwebengine-devel
BuildRequires:  vlc-devel
BuildRequires:  boost-devel
BuildRequires:  SDL2-devel
BuildRequires:  ffmpeg-free-devel
Requires:       vlc
Requires:       ffmpeg-free
Requires:       boost
Requires:       qt6-qtbase
Requires:       qt6-qtmultimedia
Requires:       qt6-qtwebengine

%description
mfaomp is a media player designed to play multiple video or audio files
simultaneously, providing a unique viewing experience for comparisons
or multi-stream playback.

%prep
%autosetup -n %{name}-%{version}
mkdir -p third_party
mkdir -p third_party/libvlcpp
tar -xf %{SOURCE1} -C third_party/libvlcpp --strip-components=1

%build
%cmake -DCMAKE_BUILD_TYPE=Release \
       -DUSE_PREDOWNLOADED_LIBVLCPP=ON \
       -DUSE_FETCHED_LIBVLCPP=OFF \
       -DENABLE_CATCH=OFF
%cmake_build

%install
%cmake_install

%files
%{_bindir}/mfaomp
%license %{_docdir}/%{name}/LICENSE.txt
%{_datadir}/applications/mfaomp.desktop

%changelog
%autochangelog