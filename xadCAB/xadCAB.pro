#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:22:36
#
#-------------------------------------------------

QT       -= gui

TARGET = xadCAB
TEMPLATE = lib

DEFINES += XADCAB_LIBRARY

SOURCES += xadCAB.cpp

HEADERS += xadCAB.h\
        xadCAB_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1B46CF7
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xadCAB.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
