#-------------------------------------------------
#
# Project created by QtCreator 2011-05-15T02:59:32
#
#-------------------------------------------------

QT       += core gui

TARGET = qXpkTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../qXpkLib
LIBS += -lqXpkLib

CONFIG (debug, debug|release) 
{
    LIBS += -L../qXpkLib-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qXpkLib-build-desktop/release
}
