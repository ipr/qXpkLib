#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:24:15
#
#-------------------------------------------------

QT       -= gui

TARGET = xadTar
TEMPLATE = lib

DEFINES += XADTAR_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadTar.cpp

HEADERS += xadTar.h\
        xadTar_global.h

