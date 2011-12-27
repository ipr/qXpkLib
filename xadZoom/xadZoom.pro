#-------------------------------------------------
#
# Project created by QtCreator 2011-10-30T04:53:32
#
#-------------------------------------------------

QT       -= gui

TARGET = xadZoom
TEMPLATE = lib

DEFINES += XADZOOM_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadZoom.cpp \
    UnZoom.cpp

HEADERS += xadZoom.h\
        xadZoom_global.h \
    UnZoom.h \
    Crc16.h




