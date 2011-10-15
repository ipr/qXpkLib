#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:18:54
#
#-------------------------------------------------

QT       -= gui

TARGET = xadZAP
TEMPLATE = lib

DEFINES += XADZAP_LIBRARY

SOURCES += xadZAP.cpp

HEADERS += xadZAP.h\
        xadZAP_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE183B51F
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xadZAP.dll
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
