#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:21:20
#
#-------------------------------------------------

QT       -= gui

TARGET = xadBzip2
TEMPLATE = lib

DEFINES += XADBZIP2_LIBRARY

SOURCES += xadBzip2.cpp

HEADERS += xadBzip2.h\
        xadBzip2_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1165CF5
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xadBzip2.dll
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
