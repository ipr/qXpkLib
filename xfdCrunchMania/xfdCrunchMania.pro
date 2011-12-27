#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T02:27:34
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdCrunchMania
TEMPLATE = lib

DEFINES += XFDCRUNCHMANIA_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += xfdCrunchMania.cpp \
    DeCrunchMania.cpp

HEADERS += xfdCrunchMania.h\
        xfdCrunchMania_global.h \
    DeCrunchMania.h

