# Copyright 2014 Juan R Cabral
#
# Module: PlastilinaCore 
# Description: 
#   Build system for PlastilinaCore library. Main purpose of the library is to be
#   a framework for 3D graphics and physics.

# Sources for the project
set(PLASTILINACORE_SOURCES 
	./src/BOManager.cpp
	./src/BufferObject.cpp
	./src/Camera.cpp
	./src/Context.cpp
	./src/Document.cpp
	./src/Face.cpp
	./src/FlatRenderer.cpp
	./src/geometry/Ray.cpp
	./src/geometry/Sphere.cpp
	./src/HEdge.cpp
	./src/IIntegrator.cpp
	./src/material/PhongMaterial.cpp
	./src/material/PointMaterial.cpp
	./src/Material.cpp
	./src/Mesh.cpp
	./src/Octree.cpp
	./src/opencl/CLUtils.cpp
	./src/opencl/OCLManager.cpp
	./src/opengl/GlException.cpp
	./src/opengl/GlslProgram.cpp
	./src/opengl/GlslShader.cpp
	./src/opengl/Sampler.cpp
	./src/opengl/Texture.cpp
	./src/opengl/VertexArrayObject.cpp
	./src/physics/Actor.cpp
	./src/physics/ForceFunctors.cpp
	./src/physics/SimSystem.cpp
	./src/Picking.cpp
	./src/PickingFacesRenderer.cpp
	./src/Plane.cpp
	./src/Plastilina.cpp
	./src/pointcloud/PointCloud.cpp
	./src/pointcloud/PointCloudRenderable.cpp
	./src/PointRenderer.cpp
	./src/Quad.cpp
	./src/QuadTree.cpp
	./src/RendererFactory.cpp
    ./src/RenderState.cpp
	./src/ResourcesManager.cpp
	./src/Scene.cpp
	./src/SceneNode.cpp
	./src/SmoothRenderer.cpp
	./src/SpEnums.cpp
	./src/Stable.cpp
	./src/subdivision/Box.cpp
	./src/subdivision/Sphere.cpp
	./src/subdivision/Subdivision.cpp
	./src/subdivision/SubdivisionRenderable.cpp
	./src/Utilities.cpp
	./src/Vertex.cpp
	./src/WireframeRenderer.cpp
)

if (APPLE)
	set(PLASTILINACORE_SOURCES_OBJCXX ./src/Context_mac.mm)
	set_source_files_properties(${PLASTILINACORE_SOURCES_OBJCXX} PROPERTIES 
	                            COMPILE_FLAGS "-x objective-c++")
elseif(UNIX)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/Context_unix.cpp)
elseif(WIN32)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/Context_win32.cpp)
endif()

# Public headers. These headers are meant to be distributed to users of the engine
set(PLASTILINACORE_HEADERS_ROOT ./include/PlastilinaCore)
set(PLASTILINACORE_PUBLIC_HEADERS 
	${PLASTILINACORE_HEADERS_ROOT}/BOManager.h 
	${PLASTILINACORE_HEADERS_ROOT}/BufferObject.h
	${PLASTILINACORE_HEADERS_ROOT}/Camera.h
	${PLASTILINACORE_HEADERS_ROOT}/Color.h
	${PLASTILINACORE_HEADERS_ROOT}/Context.h
	${PLASTILINACORE_HEADERS_ROOT}/Context_private.h
	${PLASTILINACORE_HEADERS_ROOT}/data/Functors.h
	${PLASTILINACORE_HEADERS_ROOT}/Document.h
	${PLASTILINACORE_HEADERS_ROOT}/Face.h
	${PLASTILINACORE_HEADERS_ROOT}/FlatRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/FrameBufferObject.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Aabb.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Plane.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Ray.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Sphere.h
	${PLASTILINACORE_HEADERS_ROOT}/HEdge.h
	${PLASTILINACORE_HEADERS_ROOT}/ICollection.h
	${PLASTILINACORE_HEADERS_ROOT}/IDocument.h
	${PLASTILINACORE_HEADERS_ROOT}/IIntegrator.h
	${PLASTILINACORE_HEADERS_ROOT}/IIterator.h
	${PLASTILINACORE_HEADERS_ROOT}/InstanceID.h
	${PLASTILINACORE_HEADERS_ROOT}/IRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/ISurface.h
	${PLASTILINACORE_HEADERS_ROOT}/Logging.h
	${PLASTILINACORE_HEADERS_ROOT}/material/PhongMaterial.h
	${PLASTILINACORE_HEADERS_ROOT}/material/PointMaterial.h
	${PLASTILINACORE_HEADERS_ROOT}/Material.h
	${PLASTILINACORE_HEADERS_ROOT}/math/Utils.h
	${PLASTILINACORE_HEADERS_ROOT}/Mesh.h
	${PLASTILINACORE_HEADERS_ROOT}/Octree.h
	${PLASTILINACORE_HEADERS_ROOT}/OctreeNode.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/CLUtils.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/OCLManager.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/OpenCL.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlException.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlslProgram.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlslShader.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Object.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/OpenGL.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Sampler.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Texture.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/VertexArrayObject.h
	${PLASTILINACORE_HEADERS_ROOT}/PEngineTypes.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/Actor.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/ForceFunctors.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/SimSystem.h
	${PLASTILINACORE_HEADERS_ROOT}/Picking.h
	${PLASTILINACORE_HEADERS_ROOT}/PickingFacesRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/Plastilina.h
	${PLASTILINACORE_HEADERS_ROOT}/Point3D.h
	${PLASTILINACORE_HEADERS_ROOT}/pointcloud/PointCloud.h
	${PLASTILINACORE_HEADERS_ROOT}/pointcloud/PointCloudRenderable.h
	${PLASTILINACORE_HEADERS_ROOT}/PointRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/Quad.h
	${PLASTILINACORE_HEADERS_ROOT}/Quadtree.h
	${PLASTILINACORE_HEADERS_ROOT}/RendererFactory.h
    ${PLASTILINACORE_HEADERS_ROOT}/RenderState.h
	${PLASTILINACORE_HEADERS_ROOT}/ResourcesManager.h
	${PLASTILINACORE_HEADERS_ROOT}/Scene.h
	${PLASTILINACORE_HEADERS_ROOT}/SceneNode.h
	${PLASTILINACORE_HEADERS_ROOT}/SmoothRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/SpEnums.h
	${PLASTILINACORE_HEADERS_ROOT}/Stable.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Box.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Sphere.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Subdivision.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/SubdivisionRenderable.h
	${PLASTILINACORE_HEADERS_ROOT}/Utilities.h
	${PLASTILINACORE_HEADERS_ROOT}/Vector.h
	${PLASTILINACORE_HEADERS_ROOT}/Vertex.h
	${PLASTILINACORE_HEADERS_ROOT}/WireframeRenderer.h
)

# Resources embedded in the framework. 
# Texturex, shader sources, 3D meshes, etc.
set(RESOURCES
	./shaders/Phong.vs
	./shaders/Phong.fs
	./shaders/Point.vs
	./shaders/Point.fs
)

# Library target
if (WIN32)
	add_library(PlastilinaCore STATIC
		${PLASTILINACORE_SOURCES}
		${PLASTILINACORE_SOURCES_OBJCXX}
		${PLASTILINACORE_PUBLIC_HEADERS}
		${RESOURCES}
	)
else()
	add_library(PlastilinaCore SHARED
		${PLASTILINACORE_SOURCES}
		${PLASTILINACORE_SOURCES_OBJCXX}
		${PLASTILINACORE_PUBLIC_HEADERS}
		${RESOURCES}
	)

endif()

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)
target_link_libraries(PlastilinaCore ${OPENGL_LIBRARIES})
target_link_libraries(PlastilinaCore ${OpenCL_LIBRARIES})
target_link_libraries(PlastilinaCore ${GLEW_LIBRARIES})
target_link_libraries(PlastilinaCore ${ASSIMP_LIBRARIES})

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system log date_time thread)
target_link_libraries(PlastilinaCore ${Boost_LIBRARIES})

#find_package(Subversion)
#if(SUBVERSION_FOUND)
#    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
#    message("Current revision is ${Project_WC_REVISION}")
#    Subversion_WC_LOG(${PROJECT_SOURCE_DIR} Project)
#    message("Last changed log is ${Project_LAST_CHANGED_LOG}")
#endif()

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	PATH /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

include_directories(include 
	${EIGEN_INCLUDE_DIRS} 
	../thirdparty/OpenCL
	${OPENGL_INCLUDE_DIRS}
	${OpenCL_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
	)
	
set_property(TARGET PlastilinaCore PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE)

install(TARGETS PlastilinaCore
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

if (APPLE)
	set_property(TARGET PlastilinaCore PROPERTY COMPILE_OPTIONS -std=c++11 -stdlib=libc++ -include ${CMAKE_CURRENT_SOURCE_DIR}/include/PlastilinaCore/Stable.h
	)
	set_property(TARGET PlastilinaCore 
		PROPERTY MACOSX_FRAMEWORK_IDENTIFIER com.plastilinalabs.PlastilinaCore
	)
	set_property(TARGET PlastilinaCore PROPERTY FRAMEWORK true)
	# set_property(TARGET PlastilinaCore PROPERTY RESOURCE ./shaders)
	# Copy headers preserving the file hierarchy
	foreach(HEADER ${PLASTILINACORE_PUBLIC_HEADERS})
		get_filename_component(NEW_PATH ${HEADER} DIRECTORY)
		string(REPLACE ${PLASTILINACORE_HEADERS_ROOT} "" NEW_PATH ${NEW_PATH})
		set_property(SOURCE ${HEADER} PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NEW_PATH})
	endforeach(HEADER)
	
	# Copy headers preserving the file hierarchy
	foreach(RES ${RESOURCES})
		get_filename_component(NEW_PATH ${RES} DIRECTORY)
		set_property(SOURCE ${RES} PROPERTY MACOSX_PACKAGE_LOCATION Resources/${NEW_PATH})
	endforeach(RES)
	
	find_library(FOUNDATION_LIBRARIES CoreFoundation )
	find_library(COCOA_LIBRARIES Cocoa )
	target_link_libraries(PlastilinaCore ${FOUNDATION_LIBRARIES})
	target_link_libraries(PlastilinaCore ${COCOA_LIBRARIES})
elseif (UNIX)
	set_property(TARGET PlastilinaCore 
		PROPERTY COMPILE_FLAGS -std=c++11 -include ${CMAKE_CURRENT_SOURCE_DIR}/include/PlastilinaCore/Stable.h
	)
elseif (WIN32)
	set_property(TARGET PlastilinaCore 
		PROPERTY COMPILE_FLAGS /DPLASTILINACORE_EXPORT
	)
else()
	Message("Unsupported platform")		
endif()


