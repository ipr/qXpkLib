#-------------------------------------------------
#
# Project created by QtCreator 2011-05-15T02:59:32
#
#-------------------------------------------------

QT       += core gui

# for Qt5 support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qXpkTool
TEMPLATE = app

!win32
{
    # GCC4
    gcc:QMAKE_CXXFLAGS += -std=c++0x
}

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
