TEMPLATE = app
TARGET = harbour-communi
CONFIG += c++11 sailfishapp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
QT += dbus
VERSION = 0.4
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

!exists(src/shared/shared.pri)|!exists(src/backend/src/src.pri): \
    error(A Git submodule is missing. Run \'git submodule update --init\' in the project root.)

include(src/src.pri)
include(src/shared/shared.pri)
include(src/backend/src/src.pri)

RESOURCES += \
    harbour-communi.qrc

OTHER_FILES += \
    ui/*.qml \
    ui/cover/*.qml \
    ui/dialogs/*.qml \
    ui/settings/*.qml \
    ui/view/*.qml \
    rpm/harbour-communi.spec \
    harbour-communi.desktop \
    harbour-communi.png \
    README.md
