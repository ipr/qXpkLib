#-------------------------------------------------
#
# Project created by QtCreator 2011-10-12T23:15:47
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdZCompress
TEMPLATE = lib

DEFINES += XFDZCOMPRESS_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdZCompress.cpp \
    UnZCompress.cpp

HEADERS += xfdZCompress.h\
        xfdZCompress_global.h \
    UnZCompress.h



