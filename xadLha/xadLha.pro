#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T01:09:03
#
#-------------------------------------------------

QT       -= gui

TARGET = xadLha
TEMPLATE = lib

DEFINES += XADLHA_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

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



