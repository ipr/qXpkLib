#-------------------------------------------------
#
# Project created by QtCreator 2011-10-18T22:53:00
#
#-------------------------------------------------

QT       -= gui

TARGET = xadARJ
TEMPLATE = lib

DEFINES += XADARJ_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadARJ.cpp \
    UnARJ.cpp \
    Decode.cpp \
    CRCsum.cpp

HEADERS += xadARJ.h\
        xadARJ_global.h \
    GenericTime.h \
    UnARJ.h \
    Decode.h \
    CRCsum.h \
    FilemodeFlags.h









