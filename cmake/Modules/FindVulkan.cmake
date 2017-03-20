FIND_PACKAGE(PackageHandleStandardArgs)

find_library(Vulkan_LIBRARIES 
    NAMES vulkan-1
    DOC "Vulkan graphics API"
	HINTS "$ENV{VULKAN_SDK}/Bin32")


find_path(Vulkan_INCLUDE_DIRS vulkan/vulkan.h DOC "Include directory for Vulkan"
	HINTS "$ENV{VULKAN_SDK}/include")

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Vulkan DEFAULT_MSG Vulkan_LIBRARIES)
