#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T09:51:20
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkFAST
TEMPLATE = lib

DEFINES += XPKFAST_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkFAST.cpp \
    FAST.cpp

HEADERS += xpkFAST.h\
        xpkFAST_global.h \
    FAST.h



