cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(ExternalProject)
include(cotire)

#link to qtmain policy on windows
cmake_policy(SET CMP0020 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project (QSculpt)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( ENV{BOOST_LIBRARYDIR} "$ENV{BOOST_ROOT}/lib64" )
	set( ENV{Qt5_DIR} "$ENV{Qt5_DIR}/msvc2015_64")
	set(CMAKE_LIBRARY_ARCHITECTURE x64)
else()
    set( ENV{BOOST_LIBRARYDIR} "$ENV{BOOST_ROOT}/lib" )
	set( ENV{Qt5_DIR} "$ENV{Qt5_DIR}/msvc2015")
	set(CMAKE_LIBRARY_ARCHITECTURE x86)
endif() 

option(Vulkan_ENABLED "True if Vulkan support should be enabled." ON)

find_package(Qt5 REQUIRED Core Widgets OpenGL)
find_package(VULKAN)

set(CMAKE_CXX_STANDARD 14) # C++14...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

add_subdirectory(plastilinacore)
add_subdirectory(testapp)
add_subdirectory(qsculpt)

