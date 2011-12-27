#-------------------------------------------------
#
# Project created by QtCreator 2011-10-12T20:34:30
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdSZDD
TEMPLATE = lib

DEFINES += XFDSZDD_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdSZDD.cpp \
    SzddExpand.cpp

HEADERS += xfdSZDD.h\
        xfdSZDD_global.h \
    SzddExpand.h



