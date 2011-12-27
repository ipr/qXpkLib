#-------------------------------------------------
#
# Project created by QtCreator 2011-10-28T05:53:37
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkSHSC
TEMPLATE = lib

DEFINES += XPKSHSC_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkSHSC.cpp

HEADERS += xpkSHSC.h\
        xpkSHSC_global.h

