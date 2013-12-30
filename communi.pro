TARGET = communi
CONFIG += sailfishapp

include(src/src.pri)
include(src/shared/shared.pri)
include(src/backend/src/src.pri)

OTHER_FILES += qml/communi.qml \
    qml/cover/CoverPage.qml \
    qml/dialogs/AboutDialog.qml \
    qml/dialogs/ConnectDialog.qml \
    qml/dialogs/JoinDialog.qml \
    qml/dialogs/QueryDialog.qml \
    qml/pages/BufferDelegate.qml \
    qml/pages/ChatPage.qml \
    qml/pages/MainPage.qml \
    rpm/communi.spec \
    rpm/communi.yaml \
    communi.desktop
