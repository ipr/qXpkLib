#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T01:09:03
#
#-------------------------------------------------

QT       -= gui

TARGET = xadLha
TEMPLATE = lib

DEFINES += XADLHA_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

# GCC4
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../qXpkLib

SOURCES += xadLha.cpp \
    LhArchive.cpp \
    crcio.cpp \
    LhHeader.cpp \
    LhExtract.cpp \
    LhDecoder.cpp \
    Huffman.cpp

HEADERS += xadLha.h \
        xadLha_global.h \
    LhArchive.h \
    GenericTime.h \
    LhaTypeDefs.h \
    crcio.h \
    FiletimeHelper.h \
    LhHeader.h \
    LhExtract.h \
    LhDecoder.h \
    Huffman.h \
    FilemodeFlags.h



