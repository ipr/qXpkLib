#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T01:09:34
#
#-------------------------------------------------

QT       -= gui

TARGET = xadLZX
TEMPLATE = lib

DEFINES += XADLZX_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadLZX.cpp \
    UnLzx.cpp \
    CrcSum.cpp

HEADERS += xadLZX.h\
        xadLZX_global.h \
    UnLzx.h \
    CrcSum.h



