#-------------------------------------------------
#
# Project created by QtCreator 2011-10-23T14:31:25
#
#-------------------------------------------------

QT       -= gui

TARGET = xadDMS
TEMPLATE = lib

DEFINES += XADDMS_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadDMS.cpp \
    UnDMS.cpp

HEADERS += xadDMS.h\
        xadDMS_global.h \
    UnDMS.h







