#-------------------------------------------------
#
# Project created by QtCreator 2011-11-06T08:39:10
#
#-------------------------------------------------

QT       -= gui

TARGET = xadDImp
TEMPLATE = lib

DEFINES += XADDIMP_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadDImp.cpp

HEADERS += xadDImp.h\
        xadDImp_global.h

