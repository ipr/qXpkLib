#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T09:49:15
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkDUKE
TEMPLATE = lib

DEFINES += XPKDUKE_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkDUKE.cpp

HEADERS += xpkDUKE.h\
        xpkDUKE_global.h

