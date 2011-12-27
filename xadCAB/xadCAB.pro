#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:22:36
#
#-------------------------------------------------

QT       -= gui

TARGET = xadCAB
TEMPLATE = lib

DEFINES += XADCAB_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadCAB.cpp

HEADERS += xadCAB.h\
        xadCAB_global.h

