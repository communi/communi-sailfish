######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/RowsJoinerProxy.h
HEADERS += $$PWD/simplecrypt.h

SOURCES += $$PWD/RowsJoinerProxy.cpp
SOURCES += $$PWD/simplecrypt.cpp

#lessThan(QT_MINOR_VERSION, 2) {
    HEADERS += $$PWD/qqmlsettings_p.h
    SOURCES += $$PWD/qqmlsettings.cpp
#}
