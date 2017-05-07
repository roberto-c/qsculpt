
set(TESTAPP_SOURCES 
	./BaseTest.cpp
	./CanvasTest.cpp
	./CLRender.cpp
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
	./CLRender.h
	./DocumentModelTest.h
	./ParticleSystem.h
	./PrimitiveFactory.h
	./SceneNodeTest.h
	./stable.h
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
	./shaders/Render.cl
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

add_executable(PTestApp MACOSX_BUNDLE
	${TESTAPP_SOURCES}
	${TESTAPP_PRIVATE_HEADERS}
	${RESOURCES}
)

if (APPLE)
	set_property(TARGET PTestApp PROPERTY COMPILE_OPTIONS -include ${CMAKE_CURRENT_SOURCE_DIR}/stable.h )
elseif (UNIX)
    set_property(TARGET PlastilinaCore
    	PROPERTY COMPILE_FLAGS -include ${CMAKE_CURRENT_SOURCE_DIR}/stable.h
    )
endif()

set_property(TARGET PTestApp PROPERTY RESOURCE ${RESOURCES})
set_property(TARGET PTestApp PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system log date_time thread program_options)

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	PATH /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

target_include_directories(PTestApp 
	PUBLIC 
		include 
		${EIGEN_INCLUDE_DIRS} 
		../thirdparty/OpenCL
		${CMAKE_SOURCE_DIR}/plastilinacore/include
		${Boost_INCLUDE_DIRS}
		${OpenCL_INCLUDE_DIRS}
	)

if (APPLE)	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(PTestApp ${FOUNDATION_LIBRARIES})
	target_link_libraries(PTestApp ${COCOA_LIBRARIES})
elseif(WIN32)
	target_link_libraries(PTestApp winmm.lib version.lib imm32.lib)
endif(APPLE)

target_link_libraries(PTestApp ${OPENGL_LIBRARIES}
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
get_target_property(RT_OUTPUT PTestApp RUNTIME_OUTPUT_DIRECTORY)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.cfg DESTINATION ${RT_OUTPUT}/Debug)

cotire(PTestApp)
