
TEMPLATE = app
TARGET = harbour-communi
CONFIG += sailfishapp
VERSION = 0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include(src/src.pri)
include(src/shared/shared.pri)
include(src/backend/src/src.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/cover/*.qml \
    qml/dialogs/*.qml \
    qml/misc/*.qml \
    rpm/harbour-communi.spec \
    rpm/harbour-communi.yaml \
    harbour-communi.desktop \
    harbour-communi.png \
    harbour-communi.svg \
    README.md

CONFIG += c++11

RESOURCES += \
    sailfish-ui.qrc
