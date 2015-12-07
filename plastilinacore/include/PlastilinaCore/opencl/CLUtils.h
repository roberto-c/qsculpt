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

namespace core
{
namespace utils
{
    template<>
    DLLEXPORT void convert_to(const Eigen::Vector3f & d, cl_float4 & to);
    template<>
    DLLEXPORT void convert_to(const Eigen::Vector2f & d, cl_float2 & to);
    template<>
    DLLEXPORT void convert_to(const Eigen::Vector4f & d, cl_float4 & to);
    template<>
    DLLEXPORT void convert_to(const cl_float4 & from, Eigen::Vector4f & to);
    template<>
    DLLEXPORT void convert_to(const cl_float4 & from, Eigen::Vector3f & to);
};
};

#endif
