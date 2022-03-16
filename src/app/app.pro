TEMPLATE = app
TARGET = harbour-communi
CONFIG += c++11 sailfishapp_nodata
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
QT += dbus
VERSION = 1.0-alpha
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../backend/include/IrcCore
INCLUDEPATH += ../backend/include/IrcModel
INCLUDEPATH += ../backend/include/IrcUtil
LIBS += -L ../backend/lib -lIrcCore -lIrcModel -lIrcUtil

INCLUDEPATH += ../plugins
DEPENDPATH += ../plugins

HEADERS += $$PWD/activitymodel.h
HEADERS += $$PWD/aboutdata.h
HEADERS += $$PWD/bufferfiltermodel.h
HEADERS += $$PWD/bufferproxymodel.h
HEADERS += $$PWD/messagefilter.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/messagemodel.h
HEADERS += $$PWD/messagerole.h
HEADERS += $$PWD/messagestorage.h
HEADERS += $$PWD/pluginloader.h
HEADERS += $$PWD/stringfiltermodel.h
HEADERS += $$PWD/settingsproxy.h

SOURCES += $$PWD/activitymodel.cpp
SOURCES += $$PWD/aboutdata.cpp
SOURCES += $$PWD/bufferfiltermodel.cpp
SOURCES += $$PWD/bufferproxymodel.cpp
SOURCES += $$PWD/messagefilter.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/messagemodel.cpp
SOURCES += $$PWD/messagestorage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/stringfiltermodel.cpp
SOURCES += $$PWD/settingsproxy.cpp

include($$top_srcdir/src/3rdparty/3rdparty.pri)
include($$top_srcdir/src/shared/shared.pri)
include($$top_srcdir/QMakeFiles/rpmquery.pri)

!no_resources {
    RESOURCES += ../../harbour-communi.qrc
} else {
    DEFINES += NO_RESOURCES
    qmlfiles.files = ../../qml/*
    qmlfiles.path = /usr/share/$${TARGET}/qml
    INSTALLS += qmlfiles
}

QMAKE_SUBSTITUTES = $$PWD/aboutdata.cpp.in
PKGCONFIG += mlite5

RPM_DESCRIPTION = $$rpmquery_appspec("Description")
RPM_NAME = $$rpmquery_appspec("Name")
RPM_VERSION = $$rpmquery_appspec("Version")
RPM_URL = $$rpmquery_appspec("URL")
