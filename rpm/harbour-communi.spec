
# >> macros
%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^libIrcCore|libIrcModel|libIrcUtil|libcommuniplugin|libquasselplugin.*$
# << macros

Name:       harbour-communi
Version:    0.10
Summary:    The IRC client for Sailfish OS
License:    LGPLv2 and BSD
URL:        http://communi.github.io/
Source0:    %{name}-%{version}.tar.gz
Release:    1
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(sailfishapp) >= 0.0.10
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  qt5-qttools-linguist
BuildRequires:  python3-cairosvg

%description
A simple and elegant IRC client for Sailfish OS, based on Qt and the Communi IRC framework.

%prep
%autosetup

%build
%qtc_qmake5
make %{?_smp_mflags}

%install
make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}/*
%attr(644,root,root) %{_datadir}/applications/%{name}.desktop
%attr(644,root,root) %{_datadir}/icons/hicolor/*/apps/%{name}{.png,.svg}
