FIND_PACKAGE(PackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

if (APPLE)
	find_library(SDL2IMAGE_LIBRARIES 
    NAMES SDL2_image
    DOC "SDL2 library framework for MacOS X"
    )
	find_path(SDL2IMAGE_INCLUDE_DIRS SDL2_image/SDL_image.h DOC "Include directory for SDL2_image under MacOS X")
else (APPLE)
	find_library(SDL2IMAGE_LIBRARIES 
    NAMES SDL2_image 
    DOC "SDL2_image library"
    PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
    )
	find_path(SDL2IMAGE_INCLUDE_DIRS SDL2/SDL_image.h DOC "Include directory for SDL2_image")
endif (APPLE)

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 DEFAULT_MSG SDL2IMAGE_LIBRARIES)
