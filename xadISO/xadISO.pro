#-------------------------------------------------
#
# Project created by QtCreator 2011-10-17T05:19:28
#
#-------------------------------------------------

QT       -= gui

TARGET = xadISO
TEMPLATE = lib

DEFINES += XADISO_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadISO.cpp

HEADERS += xadISO.h\
        xadISO_global.h

