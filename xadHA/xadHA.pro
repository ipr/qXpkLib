#-------------------------------------------------
#
# Project created by QtCreator 2011-10-28T06:25:55
#
#-------------------------------------------------

QT       -= gui

TARGET = xadHA
TEMPLATE = lib

DEFINES += XADHA_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadHA.cpp \
    HA.cpp

HEADERS += xadHA.h\
        xadHA_global.h \
    HA.h



