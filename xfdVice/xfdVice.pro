#-------------------------------------------------
#
# Project created by QtCreator 2011-10-30T03:54:19
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdVice
TEMPLATE = lib

DEFINES += XFDVICE_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdVice.cpp \
    Vice.cpp

HEADERS += xfdVice.h\
        xfdVice_global.h \
    Vice.h

