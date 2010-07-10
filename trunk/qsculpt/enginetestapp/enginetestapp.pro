#-------------------------------------------------
#
# Project created by QtCreator 2010-07-07T22:11:10
#
#-------------------------------------------------

QT       += core gui
#QT       -= gui

TARGET = CommandLineApp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += . \
    include \
    ../qsculpt/include \
    ../engine/include \
    ../thirdparty/Eigen

macx:release {
    QMAKE_LFLAGS += -F../build/release
    LIBS += -framework engine
    DESTDIR = ../build/release
}
else unix:release {
    QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += -L../build/release
    LIBS += -lengine
    DESTDIR = ../build/release
}
macx:debug {
    QMAKE_LFLAGS += -F../build/debug
    LIBS += -framework engine
    DESTDIR = ../build/debug
}
else unix:debug {
    QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += -L../build/debug
    LIBS += -lengine
    DESTDIR = ../build/debug
}

CONFIG += precompile_header

 # Use Precompiled headers (PCH)
 PRECOMPILED_HEADER  = Stable.h

SOURCES += main.cpp

HEADERS += \
    Stable.h
