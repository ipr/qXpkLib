#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T01:43:05
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdByteKiller
TEMPLATE = lib

DEFINES += XFDBYTEKILLER_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xfdByteKiller.cpp \
    ByteKiller.cpp

HEADERS += xfdByteKiller.h\
        xfdByteKiller_global.h \
    ByteKiller.h



