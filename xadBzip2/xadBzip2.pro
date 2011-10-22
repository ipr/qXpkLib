#-------------------------------------------------
#
# Project created by QtCreator 2011-10-16T00:21:20
#
#-------------------------------------------------

QT       -= gui

TARGET = xadBzip2
TEMPLATE = lib

DEFINES += XADBZIP2_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xadBzip2.cpp \
    xadBzip2.cpp \
    huffman.c \
    decompress.c \
    bzlib.c \
    bzip2.c \
    CBzip2.cpp \
    BzDecompress.cpp

HEADERS += xadBzip2.h\
        xadBzip2_global.h \
    bzlib_private.h \
    xadBzip2.h \
    bzlib_private.h \
    bzlib.h \
    CBzip2.h \
    BZ2crc32.h \
    BZ2randtable.h \
    BzDecompress.h



















