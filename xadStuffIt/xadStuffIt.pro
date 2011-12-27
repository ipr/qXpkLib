#-------------------------------------------------
#
# Project created by QtCreator 2011-10-29T08:30:21
#
#-------------------------------------------------

QT       -= gui

TARGET = xadStuffIt
TEMPLATE = lib

DEFINES += XADSTUFFIT_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadStuffIt.cpp \
    UnStuffIt.cpp

HEADERS += xadStuffIt.h\
        xadStuffIt_global.h \
    UnStuffIt.h



