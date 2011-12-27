#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T09:53:19
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkHUFF
TEMPLATE = lib

DEFINES += XPKHUFF_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkHUFF.cpp

HEADERS += xpkHUFF.h\
        xpkHUFF_global.h

