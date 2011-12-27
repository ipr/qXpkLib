#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T09:05:29
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkNUKE
TEMPLATE = lib

DEFINES += XPKNUKE_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkNUKE.cpp \
    Nuke.cpp

HEADERS += xpkNUKE.h\
        xpkNUKE_global.h \
    Nuke.h



