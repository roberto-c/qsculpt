# Copyright 2015 Juan R Cabral
#
# Module: QScult 
# Description: 
#   Build system for PlastilinaCore library. Main purpose of the library is to be
#   a framework for 3D graphics and physics.

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
set(CMKAE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(Qt5 REQUIRED Core Widgets OpenGL)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system)

# Sources for the project
set(QSCULPT_SOURCES 
	./src/command/BrushCommand.cpp
	./src/command/BrushProperties.cpp
	./src/command/CommandBase.cpp
	./src/command/CommandManager.cpp
	./src/command/ConfigContainer.cpp
	./src/command/MeshEditCommands.cpp
	./src/command/MoveCommand.cpp
	./src/command/orbitcommand.cpp
	./src/command/SelectCommand.cpp
	./src/command/SubdivideCommand.cpp
	./src/command/TransformCameraCommand.cpp
	./src/Console.cpp
	./src/ConsoleWindow.cpp
	./src/DocumentModel.cpp
	./src/DocumentTreeWidget.cpp
	./src/DocumentView.cpp
	./src/glitem.cpp
	./src/GlView.cpp
	./src/ObjectProperties.cpp
	./src/QSculptApp.cpp
	./src/QSculptWindow.cpp
	./src/Stable.cpp
	./src/SurfaceViewController.cpp
	./src/TransformWidget.cpp
)

set(QSCULTPT_SOURCES_OBJCXX
	./src/CustomGLContext.mm
)

set(QSCULPT_PRIVATE_HEADERS 
	./include/command/BrushCommand.h
	./include/command/BrushProperties.h
	./include/command/CommandBase.h
	./include/command/CommandManager.h
	./include/command/ConfigContainer.h
	./include/command/ICommand.h
	./include/command/IConfigContainer.h
	./include/command/MeshEditCommands.h
	./include/command/MoveCommand.h
	./include/command/orbitcommand.h
	./include/command/SelectCommand.h
	./include/command/SubdivideCommand.h
	./include/command/TransformCameraCommand.h
	./include/Console.h
	./include/ConsoleWindow.h
	./include/DocumentModel.h
	./include/DocumentTreeWidget.h
	./include/DocumentView.h
	./include/glitem.h
	./include/globalDefs.h
	./include/GlView.h
	./include/IRenderable.h
	./include/ObjectProperties.h
	./include/QSculptApp.h
	./include/QSculptWindow.h
	./include/Stable.h
	./include/SurfaceViewController.h
	./include/TransformWidget.h
	./src/CustomGLContext.h
)

QT5_WRAP_UI(UISrcs
	./ui/BrushProperties.ui
	./ui/ConsoleWindow.ui
	./ui/DocumentTreeWidget.ui
	./ui/MainWindow.ui
	./ui/TransformWidget.ui
)

add_executable(QSculpt MACOSX_BUNDLE EXCLUDE_FROM_ALL
	${QSCULPT_SOURCES}
	${QSCULTPT_SOURCES_OBJCXX}
	${UISrcs}
	${QSCULPT_PRIVATE_HEADERS}
	${RESOURCES}
)

set_property(TARGET QSculpt 
		PROPERTY EXCLUDE_FROM_ALL
	)

if (APPLE)
	set_property(TARGET QSculpt 
		PROPERTY COMPILE_OPTIONS -std=c++11 -stdlib=libc++ -include ${CMAKE_CURRENT_SOURCE_DIR}/include/Stable.h
	)
elseif (UNIX)
	set_property(TARGET QSculpt 
		PROPERTY COMPILE_FLAGS -std=c++11 -include ${CMAKE_CURRENT_SOURCE_DIR}/include/Stable.h
	)
endif()

set_property(TARGET QSculpt PROPERTY RESOURCE ${RESOURCES})
set_property(TARGET QSculpt PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE)

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	PATH /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

include_directories(include 
	${EIGEN_INCLUDE_DIRS} 
	../thirdparty/OpenCL
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/qsculpt/include/command
	${CMAKE_SOURCE_DIR}/plastilinacore/include
	${Boost_INCLUDE_DIRS}
	${OPENCL_INCLUDE_DIRS}
	)

target_link_libraries(QSculpt ${OPENGL_LIBRARIES})
target_link_libraries(QSculpt ${OPENCL_LIBRARIES})
target_link_libraries(QSculpt ${Boost_LIBRARIES})
target_link_libraries(QSculpt PlastilinaCore)
target_link_libraries(QSculpt Qt5::Widgets Qt5::OpenGL)


if (APPLE)	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(QSculpt ${FOUNDATION_LIBRARIES})
	target_link_libraries(QSculpt ${COCOA_LIBRARIES})
endif(APPLE)
