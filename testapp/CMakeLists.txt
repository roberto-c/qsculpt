
set(TESTAPP_SOURCES 
	./CLRender.cpp
	./DocumentModelTest.cpp
	./IteratorTest.cpp
	./ParticleSystem.cpp
	./PhongMaterial.cpp
	./Subdivision.cpp
	./SubdivisionTest.cpp
	./test.cpp
	./TestApp.cpp
	./TestMaterial.cpp
	./SDLMain.mm
)

set(TESTAPP_PRIVATE_HEADERS 
	./CLRender.h
	./DocumentModelTest.h
	./ParticleSystem.h
	./PhongMaterial.h
	./SceneNodeTest.h
	./SDLMain.h
	./stable.h
	./Subdivision.h
	./SubdivisionTest.h
	./TestApp.h
	./TestMaterial.h
)

set(RESOURCES
	./TestMaterial.fs
	./TestMaterial.vs
	./ParticleSystem.cl
	../images/Texture01.png
)

add_executable(PTestApp MACOSX_BUNDLE
	${TESTAPP_SOURCES}
	${TESTAPP_PRIVATE_HEADERS}
	${RESOURCES}
)

if (APPLE)
	set_property(TARGET PTestApp PROPERTY COMPILE_OPTIONS -std=c++11 -stdlib=libc++ -include ./stable.h )
elseif (UNIX)
    set_property(TARGET PlastilinaCore
    	PROPERTY COMPILE_FLAGS -std=c++11 -include ${CMAKE_CURRENT_SOURCE_DIR}/stable.h
    )
endif()

set_property(TARGET PTestApp PROPERTY RESOURCE ${RESOURCES})
set_property(TARGET PTestApp PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	PATH /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

include_directories(include 
	${EIGEN_INCLUDE_DIRS} 
	../thirdparty/OpenCL
	${CMAKE_SOURCE_DIR}/plastilinacore/include
	${Boost_INCLUDE_DIRS}
	${OPENCL_INCLUDE_DIRS}
	)

target_link_libraries(PTestApp ${OPENGL_LIBRARIES})
target_link_libraries(PTestApp ${OPENCL_LIBRARIES})
target_link_libraries(PTestApp ${SDL2_LIBRARIES})
target_link_libraries(PTestApp ${SDL2IMAGE_LIBRARIES})
target_link_libraries(PTestApp ${Boost_LIBRARIES})
target_link_libraries(PTestApp PlastilinaCore)

if (APPLE)	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(PTestApp ${FOUNDATION_LIBRARIES})
	target_link_libraries(PTestApp ${COCOA_LIBRARIES})
	
endif(APPLE)

