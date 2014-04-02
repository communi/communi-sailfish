DEPENDPATH += $$PWD $$PWD/3rdparty
INCLUDEPATH += $$PWD $$PWD/3rdparty

HEADERS += $$PWD/activitymodel.h
HEADERS += $$PWD/bufferfiltermodel.h
HEADERS += $$PWD/bufferproxymodel.h
HEADERS += $$PWD/messagefilter.h
HEADERS += $$PWD/messagemodel.h
HEADERS += $$PWD/messagerole.h
HEADERS += $$PWD/messagestorage.h
HEADERS += $$PWD/networksession.h
HEADERS += $$PWD/stringfiltermodel.h
HEADERS += $$PWD/3rdparty/RowsJoinerProxy.h
HEADERS += $$PWD/3rdparty/simplecrypt.h

SOURCES += $$PWD/activitymodel.cpp
SOURCES += $$PWD/bufferfiltermodel.cpp
SOURCES += $$PWD/bufferproxymodel.cpp
SOURCES += $$PWD/messagefilter.cpp
SOURCES += $$PWD/messagemodel.cpp
SOURCES += $$PWD/messagestorage.cpp
SOURCES += $$PWD/networksession.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/stringfiltermodel.cpp
SOURCES += $$PWD/3rdparty/RowsJoinerProxy.cpp
SOURCES += $$PWD/3rdparty/simplecrypt.cpp

lessThan(QT_MINOR_VERSION, 2) {
    HEADERS += $$PWD/3rdparty/qqmlsettings_p.h
    SOURCES += $$PWD/3rdparty/qqmlsettings.cpp
}
