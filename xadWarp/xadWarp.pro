#-------------------------------------------------
#
# Project created by QtCreator 2011-11-01T06:55:59
#
#-------------------------------------------------

QT       -= gui

TARGET = xadWarp
TEMPLATE = lib

DEFINES += XADWARP_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xadWarp.cpp \
    UnWarp.cpp

HEADERS += xadWarp.h\
        xadWarp_global.h \
    UnWarp.h



