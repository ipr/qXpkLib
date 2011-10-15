#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:24:15
#
#-------------------------------------------------

QT       -= gui

TARGET = xadTar
TEMPLATE = lib

DEFINES += XADTAR_LIBRARY

SOURCES += xadTar.cpp

HEADERS += xadTar.h\
        xadTar_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE01D15E1
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xadTar.dll
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
