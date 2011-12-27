#-------------------------------------------------
#
# Project created by QtCreator 2011-10-23T18:38:24
#
#-------------------------------------------------

QT       -= gui

TARGET = xadCPIO
TEMPLATE = lib

DEFINES += XADCPIO_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadCPIO.cpp

HEADERS += xadCPIO.h\
        xadCPIO_global.h

