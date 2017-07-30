
set(TESTAPP_SOURCES 
	./BaseTest.cpp
	./CanvasTest.cpp
	./DocumentModelTest.cpp
	./IteratorTest.cpp
	./ParticleSystem.cpp
	./PrimitiveFactory.cpp
	./SubdivisionTest.cpp
	./test.cpp
	./TestApp.cpp
	./TestMaterial.cpp
)

set(TESTAPP_PRIVATE_HEADERS 
	./CameraTest.h
	./CanvasTest.h
	./DeviceSelectionTest.h
	./DocumentModelTest.h
	./ParticleSystem.h
	./PrimitiveFactory.h
	./SceneNodeTest.h
	./Stable.h
	./SubdivisionTest.h
	./TestApp.h
	./TestMaterial.h
	./BaseTest.h
)

set(RESOURCES
	./config.cfg
	./shaders/TestMaterial.fs
	./shaders/TestMaterial.vs
	./shaders/ParticleSystem.cl
	../images/Texture01.png
)

if (APPLE)
	set(TESTAPP_SOURCES 
		${TESTAPP_SOURCES} 
		./SDLMain.mm
	)
	set(TESTAPP_PRIVATE_HEADERS 
		${TESTAPP_PRIVATE_HEADERS} 
		./SDLMain.h
	)
endif()

add_executable(TestApp MACOSX_BUNDLE
	${TESTAPP_SOURCES}
	${TESTAPP_PRIVATE_HEADERS}
	${RESOURCES}
)

set_property(TARGET TestApp PROPERTY RESOURCE ${RESOURCES})
set_property(TARGET TestApp PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE BOOST_ALL_DYN_LINK)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
set(Boost_USE_STATIC_LIBS       OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system log date_time thread program_options)

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	HINTS /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

target_include_directories(TestApp 
	PUBLIC 
		../thirdparty/OpenCL
		${EIGEN_INCLUDE_DIRS} 
		${CMAKE_SOURCE_DIR}/plastilinacore/include
		${Boost_INCLUDE_DIRS}
		${OpenCL_INCLUDE_DIRS}
		${SDL2_INCLUDE_DIRS}
		${SDL2IMAGE_INCLUDE_DIRS}
	)

if (APPLE)	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(TestApp PRIVATE ${FOUNDATION_LIBRARIES})
	target_link_libraries(TestApp PRIVATE ${COCOA_LIBRARIES})
elseif(WIN32)
	target_link_libraries(TestApp PRIVATE winmm.lib version.lib imm32.lib)
endif(APPLE)

target_link_libraries(TestApp 
    PUBLIC
	${OPENGL_LIBRARIES}
	${OpenCL_LIBRARIES}
	${SDL2_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${Boost_LIBRARIES}
	PlastilinaCore
)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share/shaders)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share/images)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/share/shaders)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/share/images)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_BINARY_DIR}/share)
file(COPY ${PROJECT_SOURCE_DIR}/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share)
file(COPY ${PROJECT_SOURCE_DIR}/images DESTINATION ${PROJECT_BINARY_DIR}/share)
get_target_property(RT_OUTPUT TestApp RUNTIME_OUTPUT_DIRECTORY)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.cfg DESTINATION ${RT_OUTPUT}/Debug)

set_target_properties(TestApp PROPERTIES
    COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_SOURCE_DIR}/Stable.h"
	COTIRE_ADD_UNITY_BUILD False)
cotire(TestApp)
