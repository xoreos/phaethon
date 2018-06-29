# If you want to build the current git checkout, run "build-from-git.sh".
# If you want to build the last stable release of xoreos instead,
# build from this specfile directly.

# Globals, overridden by build script.
%global snapshot 0

# phaethon depends on packages in rpmfusion-free, much like xoreos.

Name:           phaethon
Version:        0.0.4

# This is a bit ugly but it works.
%if "%{snapshot}" == "0"
Release:        1%{?dist}
%else
Release:        1.%{snapshot}%{?dist}
%endif

Summary:        A FLOSS resource explorer for BioWare's Aurora engine games

License:        GPLv3
URL:            https://xoreos.org/
# This URL is sad, because of the lack of a named release archive.
Source0:        https://github.com/xoreos/phaethon/archive/v%{version}.tar.gz

BuildRequires:  zlib-devel, libogg-devel, openal-soft-devel, libvorbis-devel
BuildRequires:  libxml2-devel, lzma-devel, libtool, gettext-devel

BuildRequires:  qt5-devel qt5-qtbase-devel
BuildRequires:  qt5-qtbase-gui

# Boost dependencies.
BuildRequires:  boost-devel, boost-system, boost-filesystem, boost-atomic
BuildRequires:  boost-regex, boost-locale, boost-date-time boost-chrono
BuildRequires:  boost-thread

# rpmfusion-free dependencies.
BuildRequires:  libmad-devel

%description
Phaethon is a free/libre and open source (FLOSS) resource explorer for
BioWare's Aurora engine games. Phaethon is part of the xoreos project;
please see the xoreos website and its GitHub repositories for details.

%prep
%setup -q

%build

./autogen.sh
%configure --with-release=xFedora

# When building in place we want to do a make clean.
make clean

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
# We'll get the documentation manually.
rm %{buildroot}%{_pkgdocdir}/*

%files
%{_bindir}/phaethon
%doc *.md AUTHORS ChangeLog TODO
%license COPYING

%changelog
* Mon Feb 15 2016 Ben Rosser <rosser.bjr@gmail.com> 0.0.4-1
- Initial package.
