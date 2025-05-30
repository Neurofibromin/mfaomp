%global debug_package %{nil}
Name:           mfaomp
Version:        0.4.0
Release:        %autorelease
Summary:        Multiple Files At Once Media Player

License:        GPL-3.0-or-later
URL:            https://github.com/Neurofibromin/mfaomp
Source0:        %{url}/archive/refs/tags/%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtmultimedia-devel
BuildRequires:  vlc-devel
Requires:       vlc

%description
mfaomp is a media player designed to play multiple video or audio files
simultaneously, providing a unique viewing experience for comparisons
or multi-stream playback.

%prep
%autosetup -n %{name}-%{version}
%{gpgverify} --keyring='%{SOURCE2}' --signature='%{SOURCE1}' --data='%{SOURCE0}'

%build
%cmake -DCMAKE_BUILD_TYPE=Release
%cmake_build

%install
%cmake_install

%files
%{_bindir}/mfaomp
%license %{_docdir}/%{name}/LICENSE.txt
%{_datadir}/applications/mfaomp.desktop

%changelog
%autochangelog