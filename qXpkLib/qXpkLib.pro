#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T16:30:05
#
#-------------------------------------------------

QT       -= gui

TARGET = qXpkLib
TEMPLATE = lib

DEFINES += QXPKLIB_LIBRARY

# VC++2010
DEFINES += _CRT_SECURE_NO_WARNINGS

!win32
{
    # GCC4
    gcc:QMAKE_CXXFLAGS += -std=c++0x
}

SOURCES += qxpklib.cpp \
    XpkMaster.cpp \
    AnsiFile.cpp \
    XpkTags.cpp \
    IoContext.cpp \
    FileType.cpp \
    XfdMaster.cpp \
    XfdSlave.cpp \
    XpkLibrarian.cpp \
    XadMaster.cpp \
    LibMaster.cpp \
    PathHelper.cpp

HEADERS += qxpklib.h\
        qXpkLib_global.h \
    XpkMaster.h \
    xpkLibraryBase.h \
    AnsiFile.h \
    XpkProgress.h \
    XpkTags.h \
    IoContext.h \
    FileType.h \
    xpkmaster_checksum.h \
    XfdMaster.h \
    XfdSlave.h \
    XpkLibrarian.h \
    XadMaster.h \
    xadLibraryBase.h \
    xfdLibraryBase.h \
    LibMaster.h \
    XpkCapabilities.h \
    PathHelper.h \
    arcexception.h


