#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:18:27
#
#-------------------------------------------------

QT       -= gui

TARGET = xadZOO
TEMPLATE = lib

DEFINES += XADZOO_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xadZOO.cpp \
    UnZoo.cpp \
    crcsum.cpp \
    Decode.cpp

HEADERS += xadZOO.h\
        xadZOO_global.h \
    UnZoo.h \
    crcsum.h \
    GenericTime.h \
    Decode.h \
    ZooStructures.h











