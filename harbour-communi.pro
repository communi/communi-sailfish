
TEMPLATE = app
TARGET = harbour-communi
QT += quick

CONFIG += link_pkgconfig
PKGCONFIG += sailfishapp
INCLUDEPATH += /usr/include/sailfishapp

VERSION = 0.1-beta
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include(src/src.pri)

!exists(src/shared/shared.pri)|!exists(src/backend/src/src.pri):error(A Git submodule is missing. Run \'git submodule update --init\' in the project root.)
include(src/shared/shared.pri)
include(src/backend/src/src.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/cover/*.qml \
    qml/dialogs/*.qml \
    qml/misc/*.qml \
    rpm/harbour-communi.spec \
    harbour-communi.desktop \
    harbour-communi.png \
    harbour-communi.svg \
    README.md

CONFIG += c++11

RESOURCES += \
    sailfish-ui.qrc

target.path = /usr/bin
iconfile.path = /usr/share/icons/hicolor/86x86/apps
iconfile.files = harbour-communi.png
desktopfile.path = /usr/share/applications
desktopfile.files = harbour-communi.desktop
INSTALLS = target desktopfile iconfile
