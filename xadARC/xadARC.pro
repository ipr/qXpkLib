#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T22:52:33
#
#-------------------------------------------------

QT       -= gui

TARGET = xadARC
TEMPLATE = lib

DEFINES += XADARC_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadARC.cpp \
    UnARC.cpp \
    Decode.cpp

HEADERS += xadARC.h\
        xadARC_global.h \
    GenericTime.h \
    UnARC.h \
    Decode.h






