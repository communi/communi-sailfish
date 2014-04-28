TEMPLATE = app
TARGET = harbour-communi
CONFIG += c++11 sailfishapp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
QT += dbus
VERSION = 0.5
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../backend/include/IrcCore
INCLUDEPATH += ../backend/include/IrcModel
INCLUDEPATH += ../backend/include/IrcUtil
LIBS += -L ../backend/lib -lIrcCore -lIrcModel -lIrcUtil

HEADERS += $$PWD/activitymodel.h
HEADERS += $$PWD/bufferfiltermodel.h
HEADERS += $$PWD/bufferproxymodel.h
HEADERS += $$PWD/messagefilter.h
HEADERS += $$PWD/messagemodel.h
HEADERS += $$PWD/messagerole.h
HEADERS += $$PWD/messagestorage.h
HEADERS += $$PWD/networksession.h
HEADERS += $$PWD/stringfiltermodel.h

SOURCES += $$PWD/activitymodel.cpp
SOURCES += $$PWD/bufferfiltermodel.cpp
SOURCES += $$PWD/bufferproxymodel.cpp
SOURCES += $$PWD/messagefilter.cpp
SOURCES += $$PWD/messagemodel.cpp
SOURCES += $$PWD/messagestorage.cpp
SOURCES += $$PWD/networksession.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/stringfiltermodel.cpp

include(../3rdparty/3rdparty.pri)
include(../shared/shared.pri)

RESOURCES += \
    ../../harbour-communi.qrc
