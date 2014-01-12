TARGET = harbour-communi
CONFIG += sailfishapp

include(src/src.pri)
include(src/shared/shared.pri)
include(src/backend/src/src.pri)

OTHER_FILES += qml/main.qml \
    qml/PageStackScheduler.qml \
    qml/cover/CoverPage.qml \
    qml/dialogs/AboutDialog.qml \
    qml/dialogs/ConnectDialog.qml \
    qml/dialogs/JoinDialog.qml \
    qml/dialogs/QueryDialog.qml \
    qml/pages/BufferDelegate.qml \
    qml/pages/BufferListPanel.qml \
    qml/pages/BufferPage.qml \
    qml/pages/Panel.qml \
    qml/pages/PanelView.qml \
    qml/pages/TextEntry.qml \
    qml/pages/UserListPanel.qml \
    rpm/communi.spec \
    rpm/communi.yaml \
    communi.desktop

CONFIG += c++11
