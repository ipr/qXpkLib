#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T22:52:06
#
#-------------------------------------------------

QT       -= gui

TARGET = xadACE
TEMPLATE = lib

DEFINES += XADACE_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xadACE.cpp \
    UnACE.cpp

HEADERS += xadACE.h\
        xadACE_global.h \
    UnACE.h


