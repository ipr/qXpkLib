#-------------------------------------------------
#
# Project created by QtCreator 2011-05-15T00:56:17
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkDummy
TEMPLATE = lib

DEFINES += XPKDUMMY_LIBRARY 

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkdummy.cpp

HEADERS += xpkdummy.h\
        xpkDummy_global.h

