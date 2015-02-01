
# >> macros
%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
%define __provides_exclude_from ^%{_datadir}/.*$
%define __requires_exclude ^libIrcCore|libIrcModel|libIrcUtil|libcommuniplugin|libquasselplugin.*$
# << macros

Name:       harbour-communi
Summary:    The IRC client for Sailfish OS
Group:      Applications/Internet
License:    LGPLv2 and BSD
URL:        http://communi.github.io/
Source0:    %{name}-%{version}.tar.gz

Version:    1.0
Release:    alpha

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
%{_datadir}/%{name}/lib
%{_datadir}/%{name}/qml
%{_datadir}/%{name}/plugins
%attr(644,root,root) %{_datadir}/applications/%{name}.desktop
%attr(644,root,root) %{_datadir}/icons/hicolor/86x86/apps/%{name}.png

%changelog
* Sun Feb 01 2015 J-P Nurmi <jpnurmi@gmail.com> 0.9
- Improved state saving and restoring once again
- Implemented a command queue for flood protection
- Added a setting for fixed-width font (#94)
- Allow cloning networks (#97)
- Fixed STATUSMSG handling (#100)
- Added a setting for sorting views alphabetically vs. by activity (#101)
- Clarified CTCP requests and replies (#103)
- Added a setting for disabling notification sounds
- Reorganized the settings page

* Tue Jul 15 2014 J-P Nurmi <jpnurmi@gmail.com> 0.8
- Added context menu items for opening links in messages
- Tweaked wording in notifications & removed IRC formatting
- Exposed D-Bus signals for missed & highlighted messages
- Fixed copying of plain message content without formatting

* Sun Jun 22 2014 J-P Nurmi <jpnurmi@gmail.com> 0.7
- Added a day separator
- Added support for performing commands on connect
- Added a user count limit setting for showing joins/parts/quits
- Added a press effect for the welcome dialog connection items

* Thu Jun 19 2014 J-P Nurmi <jpnurmi@gmail.com> 0.6
- Fixed newly added channels not being rejoined after network interruption
- Added a short press effect delay for messages
- Fixed a crash that occurred when opening a query from the context menu
  opened from the bottom of the user list
- Added a warning when sending more than two lines
- Added /IGNORE and /UNIGNORE commands
- Removed the custom fast-scroll buttons

* Thu Apr 17 2014 J-P Nurmi <jpnurmi@gmail.com> 0.5
- Fixed the user search case sensitivity and the toolbar icon
- Made the visualization for highlights above/below the current
  view list scrolling position more accurate
- Added /CTCP <target> <command...>
- Made "unhandled" notices to be delivered to the current view instead
  of the server view
- Added contact details to the about dialog

* Mon Apr 07 2014 J-P Nurmi <jpnurmi@gmail.com> 0.4
- Added a search toolbar for channel users
- Added a subtle visualization for highlights above/below the current
  view list scrolling position
- Fixed an issue with disappearing channels/queries

* Tue Apr 01 2014 J-P Nurmi <jpnurmi@gmail.com> 0.3
- Added a search/filter toolbar for views
- Added a context menu entry for replying to channel messages
- Made auto-scrolling less aggressive
- Fixed a re-layouting issue upon orientation changes
- Fixed a state saving issue that lead to losing channels
- Added support for TOHOLED notifications :)

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
