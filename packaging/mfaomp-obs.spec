%global debug_package %{nil}
Name:           mfaomp
Version:        0.7.3
Release:        0
Summary:        Multiple Files At Once Media Player

License:        GPL-3.0-or-later
URL:            https://github.com/Neurofibromin/mfaomp
Source0:        %{url}/archive/refs/tags/v%{version}.tar.gz
Source1:        https://github.com/videolan/libvlcpp/archive/refs/heads/master.tar.gz#/libvlcpp-master.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-base-devel
BuildRequires:  qt6-multimedia-devel
BuildRequires:  qt6-webengine
BuildRequires:  qt6-webenginewidgets-devel
BuildRequires:  qt6-webenginequick-devel
BuildRequires:  qt6-webenginecore-devel
BuildRequires:  vlc-devel
BuildRequires:  boost-devel
BuildRequires:  sdl2-compat-devel
# BuildRequires:  ffmpeg-devel conflicts with vlc?
Requires:       vlc
# Requires:       ffmpeg
Requires:       boost
Requires:       qt6-base
Requires:       qt6-multimedia
Requires:       qt6-webengine

%description
mfaomp is a media player designed to play multiple video or audio files
simultaneously, providing a unique viewing experience for comparisons
or multi-stream playback.

%prep
%setup -q -n %{name}-%{version}
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
%dir %{_docdir}/../%{name}
%license %{_docdir}/../%{name}/LICENSE.txt
%{_datadir}/applications/mfaomp.desktop

%changelog
