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
	./src/Canvas.cpp
	./src/Context.cpp
	./src/Document.cpp
	./src/Face.cpp
	./src/FlatRenderer.cpp
	./src/geometry/Ray.cpp
	./src/geometry/Sphere.cpp
	./src/HEdge.cpp
	./src/IDevice.cpp
	./src/IIntegrator.cpp
	./src/material/PhongMaterial.cpp
	./src/material/PointMaterial.cpp
	./src/material/WireFrameMaterial.cpp
	./src/Material.cpp
	./src/Mesh.cpp
	./src/Octree.cpp
	./src/opencl/ClStlAllocator.cpp
	./src/opencl/CLUtils.cpp
	./src/opencl/OCLManager.cpp
	./src/opengl/FrameBufferObject.cpp
	./src/opengl/GlDevice.cpp
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
	./src/Stable.cpp
	./src/subdivision/Box.cpp
	./src/subdivision/Sphere.cpp
	./src/subdivision/Subdivision.cpp
	./src/subdivision/SubdivisionRenderable.cpp
	./src/subdivision/GpuSubdivision.cpp
	./src/subdivision/GpuSubdivisionRenderable.cpp
	./src/Utilities.cpp
	./src/Vertex.cpp
	./src/WireframeRenderer.cpp
)

if (Vulkan_FOUND)
set(PLASTILINACORE_SOURCES
	${PLASTILINACORE_SOURCES}
	./src/vulkan/Context.cpp
	./src/vulkan/VkDevice.cpp
	./src/vulkan/VkUtils.cpp
	./src/vulkan/Vulkan.cpp
)
endif()

source_group(core REGULAR_EXPRESSION
	".*"
	)

source_group(physics REGULAR_EXPRESSION
	".*\\physics\\.*"
	)


source_group(material REGULAR_EXPRESSION
	".*\\material\\.*"
	)

source_group(subdivision REGULAR_EXPRESSION
	".*\\subdivision\\.*"
	)

source_group(opengl  REGULAR_EXPRESSION
	".*\\opengl\\.*"
	)

source_group(opencl REGULAR_EXPRESSION
	".*\\opencl\\.*"
	)

if (Vulkan_FOUND)
source_group(vulkan  REGULAR_EXPRESSION
	".*\\vulkan\\.*"
	)
endif()

if (APPLE)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/Plastilina_mac.cpp)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/opengl/Context_mac.cpp)
#	set_source_files_properties(${PLASTILINACORE_SOURCES_OBJCXX} PROPERTIES 
#	                            COMPILE_FLAGS "-x objective-c++")
elseif(UNIX)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/opengl/Context_linux.cpp)
elseif(WIN32)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/opengl/Context_win32.cpp)
	set(PLASTILINACORE_SOURCES ${PLASTILINACORE_SOURCES} ./src/Plastilina_win32.cpp)
endif()

# Public headers. These headers are meant to be distributed to users of the engine
set(PLASTILINACORE_HEADERS_ROOT ./include/PlastilinaCore)
set(PLASTILINACORE_PUBLIC_HEADERS 
	${PLASTILINACORE_HEADERS_ROOT}/BOManager.h 
	${PLASTILINACORE_HEADERS_ROOT}/BufferObject.h
	${PLASTILINACORE_HEADERS_ROOT}/Camera.h
	${PLASTILINACORE_HEADERS_ROOT}/Canvas.h
	${PLASTILINACORE_HEADERS_ROOT}/Color.h
	${PLASTILINACORE_HEADERS_ROOT}/Context.h
	${PLASTILINACORE_HEADERS_ROOT}/data/Functors.h
	${PLASTILINACORE_HEADERS_ROOT}/Document.h
	${PLASTILINACORE_HEADERS_ROOT}/Face.h
	${PLASTILINACORE_HEADERS_ROOT}/FlatRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Aabb.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Plane.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Ray.h
	${PLASTILINACORE_HEADERS_ROOT}/geometry/Sphere.h
	${PLASTILINACORE_HEADERS_ROOT}/HEdge.h
	${PLASTILINACORE_HEADERS_ROOT}/ICollection.h
	${PLASTILINACORE_HEADERS_ROOT}/IDevice.h
	${PLASTILINACORE_HEADERS_ROOT}/IDocument.h
	${PLASTILINACORE_HEADERS_ROOT}/IIntegrator.h
	${PLASTILINACORE_HEADERS_ROOT}/IIterator.h
	${PLASTILINACORE_HEADERS_ROOT}/InstanceID.h
	${PLASTILINACORE_HEADERS_ROOT}/IRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/ISurface.h
	${PLASTILINACORE_HEADERS_ROOT}/Logging.h
	${PLASTILINACORE_HEADERS_ROOT}/material/PhongMaterial.h
	${PLASTILINACORE_HEADERS_ROOT}/material/PointMaterial.h
	${PLASTILINACORE_HEADERS_ROOT}/material/WireFrameMaterial.h
	${PLASTILINACORE_HEADERS_ROOT}/Material.h
	${PLASTILINACORE_HEADERS_ROOT}/math/Utils.h
	${PLASTILINACORE_HEADERS_ROOT}/Mesh.h
	${PLASTILINACORE_HEADERS_ROOT}/Octree.h
	${PLASTILINACORE_HEADERS_ROOT}/OctreeNode.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/ClStlAllocator.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/CLUtils.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/OCLManager.h
	${PLASTILINACORE_HEADERS_ROOT}/opencl/OpenCL.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Context.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/FrameBufferObject.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlDevice.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlException.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlslProgram.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/GlslShader.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Object.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/OpenGL.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Sampler.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/Texture.h
	${PLASTILINACORE_HEADERS_ROOT}/opengl/VertexArrayObject.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/Actor.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/ForceFunctors.h
	${PLASTILINACORE_HEADERS_ROOT}/physics/SimSystem.h
	${PLASTILINACORE_HEADERS_ROOT}/Picking.h
	${PLASTILINACORE_HEADERS_ROOT}/PickingFacesRenderer.h
	${PLASTILINACORE_HEADERS_ROOT}/Plastilina.h
	${PLASTILINACORE_HEADERS_ROOT}/PlastilinaDefs.h
	${PLASTILINACORE_HEADERS_ROOT}/Plastilina_priv.h
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
	${PLASTILINACORE_HEADERS_ROOT}/Stable.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Box.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Sphere.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/Subdivision.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/SubdivisionRenderable.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/GpuSubdivision.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/GpuSubdivision_private.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/GpuSubdivisionPrimitives.h
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/GpuSubdivisionRenderable.h
	${PLASTILINACORE_HEADERS_ROOT}/Utilities.h
	${PLASTILINACORE_HEADERS_ROOT}/Variant.h
	${PLASTILINACORE_HEADERS_ROOT}/Vector.h
	${PLASTILINACORE_HEADERS_ROOT}/Version.h
	${PLASTILINACORE_HEADERS_ROOT}/Vertex.h
	${PLASTILINACORE_HEADERS_ROOT}/vulkan/Context.h
	${PLASTILINACORE_HEADERS_ROOT}/vulkan/VkDevice.h
	${PLASTILINACORE_HEADERS_ROOT}/vulkan/VkUtils.h
	${PLASTILINACORE_HEADERS_ROOT}/vulkan/Vulkan.h
	${PLASTILINACORE_HEADERS_ROOT}/WireframeRenderer.h
)

# Resources embedded in the framework. 
# Texturex, shader sources, 3D meshes, etc.
set(RESOURCES
	./shaders/Canvas.cl
	./shaders/Canvas.vs
	./shaders/Canvas.fs
	./shaders/Phong.vs
	./shaders/Phong.fs
	./shaders/Point.vs
	./shaders/Point.fs
	./shaders/WireFrame.vs
	./shaders/WireFrame.fs
	./shaders/Subdivision.cl
	${PLASTILINACORE_HEADERS_ROOT}/subdivision/GpuSubdivisionPrimitives.h
)

source_group(Resources FILES
	./shaders/Canvas.cl
	./shaders/Canvas.vs
	./shaders/Canvas.fs
	./shaders/Phong.vs
	./shaders/Phong.fs
	./shaders/Point.vs
	./shaders/Point.fs
	./shaders/WireFrame.vs
	./shaders/WireFrame.fs
	./shaders/Subdivision.cl
)

# Library target
if (WIN32)
	add_library(PlastilinaCore STATIC
		${PLASTILINACORE_SOURCES}
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
target_link_libraries(PlastilinaCore PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(PlastilinaCore PUBLIC ${OpenCL_LIBRARIES})
target_link_libraries(PlastilinaCore PUBLIC ${GLEW_LIBRARIES})
target_link_libraries(PlastilinaCore PUBLIC ${ASSIMP_LIBRARIES})
if (Vulkan_FOUND)
	target_include_directories(PlastilinaCore PUBLIC ${Vulkan_INCLUDE_DIRS})
	target_link_libraries(PlastilinaCore PUBLIC ${Vulkan_LIBRARIES})
endif()

set(Boost_USE_STATIC_LIBS       OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS filesystem system log date_time program_options)
target_link_libraries(PlastilinaCore PUBLIC ${Boost_LIBRARIES})

#find_package(Subversion)
#if(SUBVERSION_FOUND)
#    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
#    message("Current revision is ${Project_WC_REVISION}")
#    Subversion_WC_LOG(${PROJECT_SOURCE_DIR} Project)
#    message("Last changed log is ${Project_LAST_CHANGED_LOG}")
#endif()

find_path(EIGEN_INCLUDE_DIRS Eigen/Eigen
	HINTS /usr/local/include/eigen3 /usr/include/eigen3
	DOC "Include directory Eigen"
)

message("OpenCL_INCLUDE_DIRS ${OpenCL_INCLUDE_DIRS}")

configure_file(config.h.in ./include/PlastilinaCore/Config.h)

target_include_directories(PlastilinaCore
	PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/include 
		${EIGEN_INCLUDE_DIRS} 
		${OPENGL_INCLUDE_DIRS}
		../thirdparty/OpenCL
		${OpenCL_INCLUDE_DIRS}
	PRIVATE
		${ASSIMP_INCLUDE_DIRS}
		${Boost_INCLUDE_DIRS}
	)
	
set_property(TARGET PlastilinaCore PROPERTY COMPILE_DEFINITIONS PLASTILINA_GL_EXCEPTON_ENABLE BOOST_ALL_DYN_LINK)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share/shaders)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/share/shaders)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/share)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_BINARY_DIR}/share)

if (APPLE)
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
	target_link_libraries(PlastilinaCore PUBLIC ${FOUNDATION_LIBRARIES})
	target_link_libraries(PlastilinaCore PUBLIC ${COCOA_LIBRARIES})
elseif (UNIX)
elseif (WIN32)
	set_property(TARGET PlastilinaCore 
		PROPERTY COMPILE_FLAGS /DPLASTILINACORE_EXPORT
	)
else()
	Message("Unsupported platform")		
endif()
set_target_properties(PlastilinaCore PROPERTIES 
	COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_SOURCE_DIR}/include/PlastilinaCore/Stable.h"
	COTIRE_ADD_UNITY_BUILD False)
cotire(PlastilinaCore)
