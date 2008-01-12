######################################################################
# Automatically generated by qmake (2.01a) Mon Mar 19 21:21:31 2007
######################################################################


QT = core gui opengl
OBJECTS_DIR = ./obj
UI_DIR=./ui
MOC_DIR=./moc
RCC_DIR=./res
VERSION=0.1.0

release:DESTDIR=../build/release
debug:DESTDIR=../build/debug

TEMPLATE = lib
CONFIG += lib_bundle
TARGET =
DEPENDPATH += . include src
INCLUDEPATH += . include

# Input
HEADERS += include/box.h \
           include/camera.h \
           include/icamera.h \
           include/iobject3d.h \
           include/mesh.h \
           include/object3d.h \
           include/octree.h \
           include/point3d.h \
           include/scene.h \
           include/spenums.h \
           include/sphere.h \
           include/stdafx.h \
           include/triangle.h \
           include/edgecontainer.h \
           include/facecontainer.h \
           include/irenderer.h \
           include/rendererfactory.h \
           include/flatrenderer.h \
           include/pointrenderer.h \
           include/wireframerenderer.h \
           include/smoothrenderer.h
SOURCES += src/box.cpp \
           src/camera.cpp \
           src/mesh.cpp \
           src/object3d.cpp \
           src/octree.cpp \
           src/scene.cpp \
           src/sphere.cpp \
           src/stdafx.cpp \
           src/rendererfactory.cpp \
           src/flatrenderer.cpp \
           src/pointrenderer.cpp \
           src/wireframerenderer.cpp \
           src/smoothrenderer.cpp