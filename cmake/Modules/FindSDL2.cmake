FIND_PACKAGE(PackageHandleStandardArgs)

if (WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  set(_arch_suffix x64)
	else()
	  set(_arch_suffix x86)
	endif()
endif()

find_library(SDL2_LIBRARY 
    NAMES SDL2 
    DOC "SDL2 library")
find_library(SDL2MAIN_LIBRARY 
    NAMES SDL2main 
    DOC "SDL2main library")

if (MSVC14)
    set(SDL2MAIN_LIBRARIES legacy_stdio_definitions.lib ${SDL2MAIN_LIBRARY})
endif()

if (SDL2_LIBRARY)
    set(SDL2_LIBRARIES ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
endif()

find_path(SDL2_INCLUDE_DIR SDL2/SDL.h DOC "Include directory for SDL2")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 DEFAULT_MSG SDL2_INCLUDE_DIR SDL2_LIBRARY)
