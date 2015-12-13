FIND_PACKAGE(PackageHandleStandardArgs)

find_library(SDL2_LIBRARIES 
    NAMES SDL2 
    DOC "SDL2 library")
find_library(SDL2MAIN_LIBRARIES 
    NAMES SDL2main 
    DOC "SDL2main library")

if (MSVC14)
    set(SDL2MAIN_LIBRARIES legacy_stdio_definitions.lib ${SDL2MAIN_LIBRARIES})
endif()

if (SDL2_LIBRARIES)
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES} ${SDL2MAIN_LIBRARIES})
endif()

find_path(SDL2_INCLUDE_DIRS SDL2/SDL.h DOC "Include directory for SDL2")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 DEFAULT_MSG SDL2_LIBRARIES)
