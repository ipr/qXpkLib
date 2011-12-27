#-------------------------------------------------
#
# Project created by QtCreator 2011-10-30T04:04:53
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdVDCO
TEMPLATE = lib

DEFINES += XFDVDCO_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdVDCO.cpp \
    VDCO.cpp

HEADERS += xfdVDCO.h\
        xfdVDCO_global.h \
    VDCO.h



