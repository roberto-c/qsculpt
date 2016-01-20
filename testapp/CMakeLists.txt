
set(TESTAPP_SOURCES 
	./CLRender.cpp
    ./ClStlAllocator.cpp
	./DocumentModelTest.cpp
	./GpuSubdivision.cpp
	./GpuSubdivisionRenderable.cpp
	./IteratorTest.cpp
	./ParticleSystem.cpp
	./PhongMaterial.cpp
	./PrimitiveFactory.cpp
	./Subdivision.cpp
	./SubdivisionTest.cpp
	./test.cpp
	./TestApp.cpp
	./TestMaterial.cpp
)

set(TESTAPP_PRIVATE_HEADERS 
	./CLRender.h
	./ClStlAllocator.h
	./DocumentModelTest.h
	./GpuSubdivision.h
	./GpuSubdivision_private.h
	./GpuSubdivisionPrimitives.h
	./GpuSubdivisionRenderable.h
	./ParticleSystem.h
	./PhongMaterial.h
	./PrimitiveFactory.h
	./SceneNodeTest.h
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
	./Render.cl
	./Subdivision.cl
	../images/Texture01.png
)

add_executable(PTestApp MACOSX_BUNDLE
	${TESTAPP_SOURCES}
	${TESTAPP_PRIVATE_HEADERS}
	${RESOURCES}
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
	set_property(TARGET PTestApp PROPERTY COMPILE_OPTIONS -std=c++11 -stdlib=libc++ -include ${CMAKE_CURRENT_SOURCE_DIR}/stable.h )
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
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system log date_time thread program_options)

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	PATH /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

include_directories(include 
	${EIGEN_INCLUDE_DIRS} 
	../thirdparty/OpenCL
	${CMAKE_SOURCE_DIR}/plastilinacore/include
	${Boost_INCLUDE_DIRS}
	${OpenCL_INCLUDE_DIRS}
	)

target_link_libraries(PTestApp ${OPENGL_LIBRARIES}
	${OpenCL_LIBRARIES}
	${SDL2_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${Boost_LIBRARIES}
	PlastilinaCore
)

if (APPLE)	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(PTestApp ${FOUNDATION_LIBRARIES})
	target_link_libraries(PTestApp ${COCOA_LIBRARIES})
	
endif(APPLE)

install(TARGETS PTestApp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RESOURCE DESTINATION share)
        
