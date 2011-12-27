#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T08:59:45
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdGZIP
TEMPLATE = lib

DEFINES += XFDGZIP_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdGZIP.cpp

HEADERS += xfdGZIP.h\
        xfdGZIP_global.h

