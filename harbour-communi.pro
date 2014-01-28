TEMPLATE = app
TARGET = harbour-communi
CONFIG += c++11 sailfishapp

VERSION = 0.1-rc
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
    ui/view/*.qml \
    rpm/harbour-communi.spec \
    harbour-communi.desktop \
    harbour-communi.png \
    harbour-communi.svg \
    README.md
