//
//  CLUtils.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/9/12.
//
//
#include <PlastilinaCore/Stable.h>

#include <fstream>
#include <sstream>
#include <string>

#include <PlastilinaCore/opencl/CLUtils.h>

namespace core
{
namespace opencl
{
std::string loadFromFile(const std::string& filename)
{
    std::string   source;
    std::ifstream file(filename);

    if (!file.good())
    {
        throw std::runtime_error(std::string("file not found: ") + filename);
    }

    std::string line;
    while (file.good())
    {
        getline(file, line);
        line.append("\n");
        source.append(line);
    }
    return source;
}

const char* errorToString(cl_int errorcode)
{
#define CLERRSTR(X)                                                          \
    case X:                                                                  \
        return #X;                                                           \
        break
    switch (errorcode)
    {
        CLERRSTR(CL_SUCCESS);
        CLERRSTR(CL_DEVICE_NOT_FOUND);
        CLERRSTR(CL_DEVICE_NOT_AVAILABLE);
        CLERRSTR(CL_COMPILER_NOT_AVAILABLE);
        CLERRSTR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        CLERRSTR(CL_OUT_OF_RESOURCES);
        CLERRSTR(CL_OUT_OF_HOST_MEMORY);
        CLERRSTR(CL_PROFILING_INFO_NOT_AVAILABLE);
        CLERRSTR(CL_MEM_COPY_OVERLAP);
        CLERRSTR(CL_IMAGE_FORMAT_MISMATCH);
        CLERRSTR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        CLERRSTR(CL_BUILD_PROGRAM_FAILURE);
        CLERRSTR(CL_MAP_FAILURE);
        CLERRSTR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
        CLERRSTR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        CLERRSTR(CL_COMPILE_PROGRAM_FAILURE);
        CLERRSTR(CL_LINKER_NOT_AVAILABLE);
        CLERRSTR(CL_LINK_PROGRAM_FAILURE);
        CLERRSTR(CL_DEVICE_PARTITION_FAILED);
        CLERRSTR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);

        CLERRSTR(CL_INVALID_VALUE);
        CLERRSTR(CL_INVALID_DEVICE_TYPE);
        CLERRSTR(CL_INVALID_PLATFORM);
        CLERRSTR(CL_INVALID_DEVICE);
        CLERRSTR(CL_INVALID_CONTEXT);
        CLERRSTR(CL_INVALID_QUEUE_PROPERTIES);
        CLERRSTR(CL_INVALID_COMMAND_QUEUE);
        CLERRSTR(CL_INVALID_HOST_PTR);
        CLERRSTR(CL_INVALID_MEM_OBJECT);
        CLERRSTR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        CLERRSTR(CL_INVALID_IMAGE_SIZE);
        CLERRSTR(CL_INVALID_SAMPLER);
        CLERRSTR(CL_INVALID_BINARY);
        CLERRSTR(CL_INVALID_BUILD_OPTIONS);
        CLERRSTR(CL_INVALID_PROGRAM);
        CLERRSTR(CL_INVALID_PROGRAM_EXECUTABLE);
        CLERRSTR(CL_INVALID_KERNEL_NAME);
        CLERRSTR(CL_INVALID_KERNEL_DEFINITION);
        CLERRSTR(CL_INVALID_KERNEL);
        CLERRSTR(CL_INVALID_ARG_INDEX);
        CLERRSTR(CL_INVALID_ARG_VALUE);
        CLERRSTR(CL_INVALID_ARG_SIZE);
        CLERRSTR(CL_INVALID_KERNEL_ARGS);
        CLERRSTR(CL_INVALID_WORK_DIMENSION);
        CLERRSTR(CL_INVALID_WORK_GROUP_SIZE);
        CLERRSTR(CL_INVALID_WORK_ITEM_SIZE);
        CLERRSTR(CL_INVALID_GLOBAL_OFFSET);
        CLERRSTR(CL_INVALID_EVENT_WAIT_LIST);
        CLERRSTR(CL_INVALID_EVENT);
        CLERRSTR(CL_INVALID_OPERATION);
        CLERRSTR(CL_INVALID_GL_OBJECT);
        CLERRSTR(CL_INVALID_BUFFER_SIZE);
        CLERRSTR(CL_INVALID_MIP_LEVEL);
        CLERRSTR(CL_INVALID_GLOBAL_WORK_SIZE);
        CLERRSTR(CL_INVALID_PROPERTY);
        CLERRSTR(CL_INVALID_IMAGE_DESCRIPTOR);
        CLERRSTR(CL_INVALID_COMPILER_OPTIONS);
        CLERRSTR(CL_INVALID_LINKER_OPTIONS);
        CLERRSTR(CL_INVALID_DEVICE_PARTITION_COUNT);

    default:
        break;
    }
    return "<UNKNOWN_CODE>";
#undef CLERRSTR
}
}; // namespace cl
}; // namespace core

namespace core
{
namespace utils
{

template <>
void convert_to<cl_float4, Eigen::Vector3f>(const Eigen::Vector3f& d,
                                            cl_float4& to)
{
    to.s[0] = d[0];
    to.s[1] = d[1];
    to.s[2] = d[2];
    to.s[3] = 0.0f;
}
template <>
void convert_to<cl_float2, Eigen::Vector2f>(const Eigen::Vector2f& d,
                                            cl_float2& to)
{
    to.s[0] = d[0];
    to.s[1] = d[1];
}

template <>
void convert_to<cl_float4, Eigen::Vector4f>(const Eigen::Vector4f& d,
                                            cl_float4& to)
{
    to.s[0] = d[0];
    to.s[1] = d[1];
    to.s[2] = d[2];
    to.s[3] = d[3];
}

template <>
void convert_to<Eigen::Vector4f, cl_float4>(const cl_float4& from,
                                            Eigen::Vector4f& to)
{
    to[0] = from.s[0];
    to[1] = from.s[1];
    to[2] = from.s[2];
    to[3] = from.s[3];
}

template <>
void convert_to<Eigen::Vector3f, cl_float4>(const cl_float4& from,
                                            Eigen::Vector3f& to)
{
    to[0] = from.s[0];
    to[1] = from.s[1];
    to[2] = from.s[2];
}

std::string to_string(const cl_float2& v)
{
    std::stringstream strout;
    strout << "(" << v.s[0] << "," << v.s[1] << ")";
    return strout.str();
}

std::string to_string(const cl_float4& v)
{
    std::stringstream strout;
    strout << "(" << v.s[0] << "," << v.s[1] << "," << v.s[2] << "," << v.s[3] << ")";
    return strout.str();
}

} // namespace utils
} // namespace core
