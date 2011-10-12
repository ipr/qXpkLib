#-------------------------------------------------
#
# Project created by QtCreator 2011-10-12T23:15:47
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdZCompress
TEMPLATE = lib

DEFINES += XFDZCOMPRESS_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xfdZCompress.cpp \
    UnZCompress.cpp

HEADERS += xfdZCompress.h\
        xfdZCompress_global.h \
    UnZCompress.h



