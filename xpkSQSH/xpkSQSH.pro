#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T09:08:19
#
#-------------------------------------------------

QT       -= gui

TARGET = xpkSQSH
TEMPLATE = lib

DEFINES += XPKSQSH_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xpkSQSH.cpp

HEADERS += xpkSQSH.h\
        xpkSQSH_global.h

