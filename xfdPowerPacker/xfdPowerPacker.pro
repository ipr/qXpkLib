#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T08:24:20
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdPowerPacker
TEMPLATE = lib

DEFINES += XFDPOWERPACKER_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdPowerPacker.cpp \
    PowerPacker.cpp

HEADERS += xfdPowerPacker.h \
        xfdPowerPacker_global.h \
    PowerPacker.h
