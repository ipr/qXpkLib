#-------------------------------------------------
#
# Project created by QtCreator 2011-10-30T03:54:19
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdVice
TEMPLATE = lib

DEFINES += XFDVICE_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xfdVice.cpp \
    Vice.cpp

HEADERS += xfdVice.h\
        xfdVice_global.h \
    Vice.h

