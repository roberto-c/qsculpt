FIND_PACKAGE(PackageHandleStandardArgs)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( _arch_dir "$ENV{VULKAN_SDK}/Bin" )
else()
    set( _arch_dir "$ENV{VULKAN_SDK}/Bin32" )
endif()

find_library(Vulkan_LIBRARIES 
    NAMES vulkan-1
    DOC "Vulkan graphics API"
	HINTS ${_arch_dir})


find_path(Vulkan_INCLUDE_DIRS vulkan/vulkan.h DOC "Include directory for Vulkan"
	HINTS "$ENV{VULKAN_SDK}/include")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Vulkan DEFAULT_MSG Vulkan_LIBRARIES)
