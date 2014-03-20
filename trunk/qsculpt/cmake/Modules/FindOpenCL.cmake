FIND_PACKAGE(PackageHandleStandardArgs)

if (APPLE)
	find_library(OPENCL_LIBRARIES OpenCL DOC "OpenCL framework for MacOS X")
	find_path(OPENCL_INCLUDE_DIRS OpenCL/cl.h DOC "Include directory for OpenCL under MacOS X")
else (APPLE)
message("Unsupported at the moment")
endif (APPLE)

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenCL DEFAULT_MSG OPENCL_LIBRARIES)
