FIND_PACKAGE(PackageHandleStandardArgs)

find_library(VULKAN_LIBRARIES 
    NAMES vulkan-1
    DOC "Vulkan graphics API"
	HINTS "$ENV{VULKAN_SDK}/Bin32")


find_path(VULKAN_INCLUDE_DIRS vulkan/vulkan.h DOC "Include directory for SDL2"
	HINTS "$ENV{VULKAN_SDK}/include")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VULKAN DEFAULT_MSG VULKAN_LIBRARIES)
