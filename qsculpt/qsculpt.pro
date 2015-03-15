# #####################################################################
# Automatically generated by qmake (2.01a) Mon Mar 19 22:10:15 2007
# #####################################################################
QT = gui \
    core \
    opengl
OBJECTS_DIR = ./obj
UI_DIR = ./ui
MOC_DIR = ./moc
RCC_DIR = ./res
TEMPLATE = app
TARGET = 
DEPENDPATH += . \
    include \
    resources \
    src \
    ui \
    include/command \
    src/command \
    ../engine/include
INCLUDEPATH += . \
    include \
    include/command \
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

CONFIG += precompile_header

 # Use Precompiled headers (PCH)
 PRECOMPILED_HEADER  = include/Stable.h

# Input
HEADERS += include/command/BrushCommand.h \
    include/command/BrushProperties.h \
    include/command/CommandBase.h \
    include/command/CommandManager.h \
    include/command/ConfigContainer.h \
    include/command/ICommand.h \
    include/command/IConfigContainer.h \
    include/command/MoveCommand.h \
    include/command/SelectCommand.h \
    include/command/SubdivideCommand.h \
    include/command/TransformCameraCommand.h \
    include/command/MeshEditCommands.h \
    include/Document.h \
    include/DocumentView.h \
    include/globalDefs.h \
    include/GlView.h \
    include/IDocument.h \
    include/IRenderable.h \
    include/ObjectProperties.h \
    include/QSculptApp.h \
    include/QSculptWindow.h \
    include/Stable.h \
    include/SurfaceViewController.h \
    include/TransformWidget.h \
    include/command/orbitcommand.h \
    include/glitem.h \
    include/ConsoleWindow.h \
    include/Console.h \
    include/DocumentTreeWidget.h

FORMS += ui/BrushProperties.ui \
    ui/ConsoleWindow.ui \
    ui/MainWindow.ui \
    ui/TransformWidget.ui \
    ui/DocumentTreeWidget.ui

SOURCES += src/command/BrushCommand.cpp \
    src/command/BrushProperties.cpp \
    src/command/CommandBase.cpp \
    src/command/CommandManager.cpp \
    src/command/ConfigContainer.cpp \
    src/command/MoveCommand.cpp \
    src/command/SelectCommand.cpp \
    src/command/SubdivideCommand.cpp \
    src/command/TransformCameraCommand.cpp \
    src/command/MeshEditCommands.cpp \
    src/Document.cpp \
    src/DocumentView.cpp \
    src/GlView.cpp \
    src/ObjectProperties.cpp \
    src/QSculptApp.cpp \
    src/QSculptWindow.cpp \
    src/Stable.cpp \
    src/SurfaceViewController.cpp \
    src/TransformWidget.cpp \
    src/command/orbitcommand.cpp \
    src/glitem.cpp \
    src/ConsoleWindow.cpp \
    src/Console.cpp \
    src/DocumentTreeWidget.cpp

RESOURCES += resources/QSculpt.qrc

OTHER_FILES += \
    resources/shaders/phong.vs \
    resources/shaders/phong.fs
