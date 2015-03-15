FIND_PACKAGE(PackageHandleStandardArgs)

find_library(SDL2_LIBRARIES SDL2 DOC "SDL2 library framework for MacOS X")
find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h DOC "Include directory for SDL2 under MacOS X")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 DEFAULT_MSG SDL2_LIBRARIES)
