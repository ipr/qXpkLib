#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:18:27
#
#-------------------------------------------------

QT       -= gui

TARGET = xadZOO
TEMPLATE = lib

DEFINES += XADZOO_LIBRARY

SOURCES += xadZOO.cpp

HEADERS += xadZOO.h\
        xadZOO_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE19458D2
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xadZOO.dll
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
