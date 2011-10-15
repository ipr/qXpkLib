#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T20:50:39
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdImploder
TEMPLATE = lib

DEFINES += XFDIMPLODER_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xfdImploder.cpp \
    ImploderExploder.cpp

HEADERS += xfdImploder.h\
        xfdImploder_global.h \
    ImploderExploder.h

