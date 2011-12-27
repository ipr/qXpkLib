#-------------------------------------------------
#
# Project created by QtCreator 2011-10-23T14:37:17
#
#-------------------------------------------------

QT       -= gui

TARGET = xadRPM
TEMPLATE = lib

DEFINES += XADRPM_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadRPM.cpp

HEADERS += xadRPM.h\
        xadRPM_global.h

