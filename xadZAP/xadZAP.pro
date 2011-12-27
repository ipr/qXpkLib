#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:18:54
#
#-------------------------------------------------

QT       -= gui

TARGET = xadZAP
TEMPLATE = lib

DEFINES += XADZAP_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadZAP.cpp \
    UnZAP.cpp

HEADERS += xadZAP.h\
        xadZAP_global.h \
    UnZAP.h



