//
//  CLUtils.h
//  
//
//  Created by Juan Roberto Cabral Flores on 12/9/12.
//
//

#ifndef _CLUtils_h
#define _CLUtils_h

#include <PlastilinaCore/opencl/OpenCL.h>
#include <PlastilinaCore/Utilities.h>

namespace opencl {
    DLLEXPORT std::string loadFromFile(const std::string & filename);
    
    DLLEXPORT const char* errorToString(cl_int errorcode);
}

#endif
