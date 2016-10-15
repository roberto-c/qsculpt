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

ExternalProject_Add(NV_VKCPP 
	PREFIX "${CMAKE_SOURCE_DIR}/external"
	GIT_REPOSITORY "https://github.com/nvpro-pipeline/vkcpp.git"
    INSTALL_COMMAND ""
)
find_package(VULKAN)

add_subdirectory(plastilinacore)
add_subdirectory(testapp)
add_subdirectory(qsculpt)

