#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T01:45:19
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdStoneCracker
TEMPLATE = lib

DEFINES += XFDSTONECRACKER_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdStoneCracker.cpp \
    StoneCracker.cpp

HEADERS += xfdStoneCracker.h\
        xfdStoneCracker_global.h \
    StoneCracker.h



