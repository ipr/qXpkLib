#-------------------------------------------------
#
# Project created by QtCreator 2011-05-19T08:24:20
#
#-------------------------------------------------

QT       -= gui

TARGET = xfdPowerPacker
TEMPLATE = lib

DEFINES += XFDPOWERPACKER_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../qXpkLib

SOURCES += xfdPowerPacker.cpp \
    PowerPacker.cpp

HEADERS += xfdPowerPacker.h \
        xfdPowerPacker_global.h \
    PowerPacker.h
