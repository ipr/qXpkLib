#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T05:25:13
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkRLEN
TEMPLATE = lib

DEFINES += XPKRLEN_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkRLEN.cpp

HEADERS += xpkRLEN.h\
        xpkRLEN_global.h

