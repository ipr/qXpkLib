#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T02:27:34
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdCrunchMania
TEMPLATE = lib

DEFINES += XFDCRUNCHMANIA_LIBRARY

SOURCES += xfdCrunchMania.cpp \
    DeCrunchMania.cpp

HEADERS += xfdCrunchMania.h\
        xfdCrunchMania_global.h \
    DeCrunchMania.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE16A27D3
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = xfdCrunchMania.dll
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


