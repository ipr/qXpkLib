#-------------------------------------------------
#
# Project created by QtCreator 2011-05-15T00:56:17
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkNONE
TEMPLATE = lib

DEFINES += XPKNONE_LIBRARY 

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkNONE.cpp

HEADERS += xpkNONE.h\
        xpkNONE_global.h

