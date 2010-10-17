#-------------------------------------------------
#
# Project created by QtCreator 2010-07-06T16:50:41
#
#-------------------------------------------------

QT       += opengl testlib
# QT       -= gui

TARGET = tst_qsculptenginetest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_qsculptenginetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += . \
    include \
    ../engine/include \
    ../thirdparty/Eigen

macx:release {
    QMAKE_LFLAGS += -F../build/release
    LIBS += -framework \
        engine
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
    LIBS += -framework \
        engine
    DESTDIR = ../build/debug
}
else unix:debug {
    QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += -L../build/debug
    LIBS += -lengine
    DESTDIR = ../build/debug
}
