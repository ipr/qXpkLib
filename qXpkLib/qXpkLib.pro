#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T16:30:05
#
#-------------------------------------------------

QT       -= gui

TARGET = qXpkLib
TEMPLATE = lib

DEFINES += QXPKLIB_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += qxpklib.cpp \
    XpkMaster.cpp \
    AnsiFile.cpp

HEADERS += qxpklib.h\
        qXpkLib_global.h \
    XpkMaster.h \
    xpkLibraryBase.h \
    AnsiFile.h \
    XpkProgress.h
