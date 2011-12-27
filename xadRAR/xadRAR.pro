#-------------------------------------------------
#
# Project created by QtCreator 2011-10-23T18:20:12
#
#-------------------------------------------------

QT       -= gui

TARGET = xadRAR
TEMPLATE = lib

DEFINES += XADRAR_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadRAR.cpp \
    UnRAR.cpp

HEADERS += xadRAR.h\
        xadRAR_global.h \
    UnRAR.h



