#-------------------------------------------------
#
# Project created by QtCreator 2012-02-12T07:47:31
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkRDCN
TEMPLATE = lib

DEFINES += XPKRDCN_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkRDCN.cpp \
    RDCN.cpp

HEADERS += xpkRDCN.h\
        xpkRDCN_global.h \
    RDCN.h
