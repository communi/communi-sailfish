QUASSELDIR = $$PWD/../../3rdparty/quassel/src/common

exists($$QUASSELDIR) {
    TEMPLATE = lib
    TARGET = quasselplugin
    CONFIG += plugin
    QT = core network

    INCLUDEPATH += ../ ../../backend/include/IrcCore
    DEPENDPATH += ../ ../../backend/include/IrcCore
    LIBS += -L../../backend/lib -lIrcCore

    target.path = /usr/share/harbour-communi/plugins
    INSTALLS += target

    HEADERS += $$PWD/quasselplugin.h
    SOURCES += $$PWD/quasselplugin.cpp

    include(protocol/quasselprotocol.pri)
}
