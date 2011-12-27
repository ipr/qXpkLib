#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T22:52:06
#
#-------------------------------------------------

QT       -= gui

TARGET = xadACE
TEMPLATE = lib

DEFINES += XADACE_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadACE.cpp \
    UnACE.cpp \
    CRCsum.cpp \
    Decompress.cpp

HEADERS += xadACE.h\
        xadACE_global.h \
    UnACE.h \
    CRCsum.h \
    Decompress.h \
    AceStructures.h










