#-------------------------------------------------
#
# Project created by QtCreator 2011-10-28T05:53:01
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkSASC
TEMPLATE = lib

DEFINES += XPKSASC_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkSASC.cpp

HEADERS += xpkSASC.h\
        xpkSASC_global.h

