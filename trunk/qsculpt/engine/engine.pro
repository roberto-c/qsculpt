# #####################################################################
# Automatically generated by qmake (2.01a) Mon Mar 19 21:21:31 2007
# #####################################################################
QT = core \
    gui \
    opengl
OBJECTS_DIR = ./obj
UI_DIR = ./ui
MOC_DIR = ./moc
RCC_DIR = ./res
VERSION = 0.1.0
QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES
release:DESTDIR = ../build/release
debug:DESTDIR = ../build/debug
TEMPLATE = lib
CONFIG += lib_bundle precompile_header
TARGET = 
DEPENDPATH += . \
    include \
    src
INCLUDEPATH += . \
    include \
    ../thirdparty/Eigen

 # Use Precompiled headers (PCH)
 PRECOMPILED_HEADER  = include/StdAfx.h

# Input
HEADERS += \
    include/Aabb.h \
    include/BOManager.h \
    include/Box.h \
    include/BufferObject.h \
    include/Camera.h \
    include/Face.h \
    include/FlatRenderer.h \
    include/HEdge.h \
    include/ICamera.h \
    include/IIterator.h \
    include/IRenderer.h \
    include/ISurface.h \
    include/Material.h \
    include/Mesh.h \
    include/Octree.h \
    include/Picking.h \
    include/PickingFacesRenderer.h \
    include/Point3D.h \
    include/PointRenderer.h \
    include/Quad.h \
    include/Quadtree.h \
    include/RendererFactory.h \
    include/Scene.h \
    include/SceneNode.h \
    include/SmoothRenderer.h \
    include/SpEnums.h \
    include/Sphere.h \
    include/StdAfx.h \
    include/Subdivision.h \
    include/Vertex.h \
    include/WireframeRenderer.h

SOURCES += src/BOManager.cpp \
    src/Box.cpp \
    src/BufferObject.cpp \
    src/Camera.cpp \
    src/Face.cpp \
    src/FlatRenderer.cpp \
    src/HEdge.cpp \
    src/Material.cpp \
    src/Mesh.cpp \
    src/Octree.cpp \
    src/Picking.cpp \
    src/PickingFacesRenderer.cpp \
    src/PointRenderer.cpp \
    src/Quad.cpp \
    src/RendererFactory.cpp \
    src/Scene.cpp \
    src/SceneNode.cpp \
    src/SmoothRenderer.cpp \
    src/SpEnums.cpp \
    src/Sphere.cpp \
    src/StdAfx.cpp \
    src/Subdivision.cpp \
    src/Vertex.cpp \
    src/WireframeRenderer.cpp
