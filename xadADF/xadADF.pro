#-------------------------------------------------
#
# Project created by QtCreator 2011-11-06T08:21:56
#
#-------------------------------------------------

QT       -= gui

TARGET = xadADF
TEMPLATE = lib

DEFINES += XADADF_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadADF.cpp \
    UnADF.cpp

HEADERS += xadADF.h\
        xadADF_global.h \
    UnADF.h



