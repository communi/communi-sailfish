
# >> macros
%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
# << macros

Name:       harbour-communi
Summary:    The IRC client for Sailfish OS
Group:      Applications/Internet
License:    LGPLv2 and BSD
URL:        http://communi.github.io/
Source0:    %{name}-%{version}.tar.gz

Version:    0.2
Release:    1

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(Qt5Core), pkgconfig(Qt5Qml), pkgconfig(Qt5Quick), pkgconfig(Qt5DBus), pkgconfig(sailfishapp) >= 0.0.10

%description
Internet Relay Chat (IRC) is a protocol for live interactive Internet text messaging (chat) or synchronous conferencing. It is mainly designed for group communication in discussion forums, called channels, but also allows one-to-one communication via private messages.

%prep
%setup -q

%build
%qtc_qmake5
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%attr(644,root,root) %{_datadir}/applications/%{name}.desktop
%attr(644,root,root) %{_datadir}/icons/hicolor/86x86/apps/%{name}.png

%changelog
* Mon Feb 17 2014 J-P Nurmi <jpnurmi@gmail.com> 0.2
- Landscape support
- User list context menu
- Performance optimizations
- Application settings (font size, show/hide events, haptic feedback)

* Tue Feb 04 2014 J-P Nurmi <jpnurmi@gmail.com> 0.1
- The first stable public release

* Tue Jan 28 2014 J-P Nurmi <jpnurmi@gmail.com> 0.1-rc
- A release candidate

* Sat Jan 18 2014 Timur Kristóf <venemo@fedoraproject.org> 0.1-beta
- Initial Sailfish beta release
