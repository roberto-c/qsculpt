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

namespace core
{
namespace cl
{
    DLLEXPORT std::string loadFromFile(const std::string & filename);

    DLLEXPORT const char* errorToString(cl_int errorcode);
};
};

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

    std::string to_string(const cl_float2 & v);
    std::string to_string(const cl_float4 & v);
};
};

inline bool operator==(const cl_float4 &lhs, const cl_float4 &rhs)
{
    return lhs.s[0] == rhs.s[0]
        && lhs.s[1] == rhs.s[1]
        && lhs.s[2] == rhs.s[2]
        && lhs.s[3] == rhs.s[3];
}
inline bool operator!=(const cl_float4 &lhs, const cl_float4 &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const cl_float2 &lhs, const cl_float2 &rhs)
{
    return lhs.s[0] == rhs.s[0]
        && lhs.s[1] == rhs.s[1];
}
inline bool operator!=(const cl_float2 &lhs, const cl_float2 &rhs)
{
    return !(lhs == rhs);
}


#endif
