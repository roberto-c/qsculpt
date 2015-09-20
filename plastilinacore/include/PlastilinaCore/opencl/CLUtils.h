//
//  CLUtils.h
//  
//
//  Created by Juan Roberto Cabral Flores on 12/9/12.
//
//

#ifndef _CLUtils_h
#define _CLUtils_h

#include "cl.hpp"
#include <PlastilinaCore/Utilities.h>

namespace opencl {
	std::string loadFromFile(const std::string & filename);
    
    const char* errorToString(cl_int errorcode);
}

#endif
