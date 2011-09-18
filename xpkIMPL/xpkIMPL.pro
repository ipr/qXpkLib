#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T20:50:39
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkIMPL
TEMPLATE = lib

DEFINES += XPKIMPL_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xpkIMPL.cpp \
    ImploderExploder.cpp

HEADERS += xpkIMPL.h\
        xpkIMPL_global.h \
    ImploderExploder.h

