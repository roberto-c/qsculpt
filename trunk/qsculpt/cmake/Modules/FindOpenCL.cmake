FIND_PACKAGE(PackageHandleStandardArgs)

if (APPLE)
	find_library(OPENCL_LIBRARIES OpenCL DOC "OpenCL framework for MacOS X")
	find_path(OPENCL_INCLUDE_DIRS OpenCL/cl.h DOC "Include directory for OpenCL under MacOS X")
elseif (UNIX)
	set(AMDAPPSDKROOT $ENV{AMDAPPSDKROOT})	
	find_library(OPENCL_LIBRARIES OpenCL
		PATH ${AMDAPPSDKROOT}/lib/${CMAKE_SYSTEM_PROCESSOR} 
		DOC "OpenCL dynamic link libraries"
	)
	find_path(OPENCL_INCLUDE_DIRS CL/cl.h 
		PATH ${AMDAPPSDKROOT}/include 
		DOC "Include directory for OpenCL"
	)
else()
	message("Unsupported at the moment")
endif (APPLE)

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenCL DEFAULT_MSG OPENCL_LIBRARIES)

