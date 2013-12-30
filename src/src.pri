DEPENDPATH += $$PWD $$PWD/3rdparty
INCLUDEPATH += $$PWD $$PWD/3rdparty

HEADERS += $$PWD/bufferproxymodel.h
HEADERS += $$PWD/messagemodel.h
HEADERS += $$PWD/messagestorage.h
HEADERS += $$PWD/networksession.h
HEADERS += $$PWD/3rdparty/RowsJoinerProxy.h

SOURCES += $$PWD/bufferproxymodel.cpp
SOURCES += $$PWD/messagemodel.cpp
SOURCES += $$PWD/messagestorage.cpp
SOURCES += $$PWD/networksession.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/3rdparty/RowsJoinerProxy.cpp

lessThan(QT_MINOR_VERSION, 2) {
    HEADERS += $$PWD/3rdparty/qqmlsettings_p.h
    SOURCES += $$PWD/3rdparty/qqmlsettings.cpp
}
