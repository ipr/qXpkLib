#-------------------------------------------------
#
# Project created by QtCreator 2012-02-12T07:44:16
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkILZR
TEMPLATE = lib

DEFINES += XPKILZR_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkILZR.cpp

HEADERS += xpkILZR.h\
        xpkILZR_global.h

