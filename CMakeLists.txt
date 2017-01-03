cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(ExternalProject)

#link to qtmain policy on windows
cmake_policy(SET CMP0020 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

project (QSculpt)

find_package(VULKAN)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( ENV{BOOST_LIBRARYDIR} "$ENV{BOOST_ROOT}/lib64" )
else()
    set( ENV{BOOST_LIBRARYDIR} "$ENV{BOOST_ROOT}/lib" )
endif() 

add_subdirectory(plastilinacore)
add_subdirectory(testapp)
add_subdirectory(qsculpt)

