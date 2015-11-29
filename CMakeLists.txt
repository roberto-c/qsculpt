cmake_minimum_required (VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#link to qtmain policy on windows
cmake_policy(SET CMP0020 NEW)
project (QSculpt)

add_subdirectory(plastilinacore)
add_subdirectory(testapp)
add_subdirectory(qsculpt)
