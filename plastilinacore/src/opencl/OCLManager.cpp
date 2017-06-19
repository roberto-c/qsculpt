//
//  OCLManager.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/Context.h>

#include <iostream>

using namespace core::opencl;

struct CLManager::Impl
{
    std::vector<cl::Platform> platforms;
    cl::Context               context;
    cl::CommandQueue          queue;
    std::vector<cl::Device>   devices;

    bool     useGpu;
    bool     initialized;
    intptr_t glCtxHnd;
#ifdef _WIN32
    HDC hdc;
#endif

    Impl()
        : useGpu(true)
        , initialized(false)
        , glCtxHnd(0)
#ifdef _WIN32
        , hdc(0)
#endif
    {
    }

    void printPlatformInfo(const cl::Platform& platform);

    void printDeviceInfo(const cl::Device& device);

    std::vector<cl::Device>
    devicesOfType(std::vector<cl::Platform>& platforms, cl_device_type type);

    std::vector<cl::Device> devicesOfType(cl::Platform&  platform,
                                          cl_device_type type);
};

CLManager* g_clManager = NULL;

CLManager* CLManager::instance()
{
    assert(g_clManager && "CLManager not initialized");
    return g_clManager;
}

bool CLManager::create()
{
    if (!g_clManager)
    {
        g_clManager = new CLManager();
    }
    assert(g_clManager && "CLManager not initialized");
    return g_clManager != nullptr;
}

bool CLManager::destroy()
{
    assert(g_clManager && "CLManager not initialized");
    g_clManager->destroy();
    return true;
}

std::vector<cl::Device> CLManager::devicesForGLContext()
{
    std::vector<cl::Device> deviceList;

    if (!d->glCtxHnd)
    {
        TRACE(debug)
            << "No OpenGL context specified. Not using CL/GL sharing.";
        return deviceList;
    }

    std::vector<cl_context_properties> prop;
    for (auto& platform : d->platforms)
    {
        try
        {
            prop.clear();
            prop.push_back(CL_CONTEXT_PLATFORM);
            prop.push_back((cl_context_properties)platform());
#ifdef __APPLE__
            prop.push_back(CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE);
            prop.push_back(d->glCtxHnd);
#endif
#ifdef _WIN32
            prop.push_back(CL_GL_CONTEXT_KHR);
            prop.push_back(d->glCtxHnd);
            prop.push_back(CL_WGL_HDC_KHR);
            prop.push_back(reinterpret_cast<cl_context_properties>(d->hdc));
#endif
            prop.push_back(0);

            size_t                    bytes = 0;
            std::vector<cl_device_id> devs;
#ifdef __APPLE__
#else
            clGetGLContextInfoKHR_fn clGetGLContextInfoKHR =
                (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddressForPlatform(
					platform(), "clGetGLContextInfoKHR");
            if (!clGetGLContextInfoKHR)
            {
                TRACE(debug)
                    << "Function pointer not found clGetGLContextInfoKHR";
                continue;
            }
            cl_int error = clGetGLContextInfoKHR(
                prop.data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr,
                &bytes);
            cl::detail::errHandler(error, "clGetGLContextInfoKHR");
            // allocating the mem
            size_t devNum = bytes / sizeof(cl_device_id);
            devs.resize(devNum);
            // reading the info
            error = clGetGLContextInfoKHR(prop.data(),
                                          CL_DEVICES_FOR_GL_CONTEXT_KHR,
                                          bytes, devs.data(), NULL);
            cl::detail::errHandler(error, "clGetGLContextInfoKHR");
#endif
            for (auto& dev : devs)
            {
                deviceList.push_back(cl::Device(dev));
            }
        }
        catch (cl::Error& e)
        {
            TRACE(error) << "Failed to get platform info";
            TRACE(error) << "OpenCL exception:" << e.err() << " ("
                         << errorToString(e.err())
                         << "): " << e.what();
            d->printPlatformInfo(platform);
        }
    }
    return deviceList;
}

bool CLManager::isExtensionSupported(const cl::Platform platform,
                                     const std::string& name) const
{
    std::string extensions;
    platform.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);

    return false;
}

CLManager::CLManager()
    : d(new Impl)
{
}

CLManager::~CLManager() {}

void CLManager::setUseGPU(bool useGPU) { d->useGpu = useGPU; }

void CLManager::setOpenGLContext(intptr_t hnd) { d->glCtxHnd = hnd; }

#ifdef _WIN32
void CLManager::setDeviceContext(HDC hdc) { d->hdc = hdc; }
#endif

bool CLManager::initializeWithGLContext(void* oglCtx, void* deviceCtx)
{
    return false;
}

bool CLManager::initializeWithGraphicsContext(core::IGraphicsContext * ctx)
{
    cl_int err = CL_SUCCESS;

    if (d->initialized)
    {
        TRACE(debug) << "Already initialized.";
        return true;
    }

    if (ctx == nullptr || ctx->contextType() != core::ContextType::OpenGL)
    {
        TRACE(error) << "OpenCL can only be used with an OpenGL context";
        return false;
    }
    gl::Context* context = static_cast<gl::Context*>(ctx);
    try
    {
        TRACE(debug) << "Initializing OpenCL...";
        std::string               value;
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0)
        {
            TRACE(debug) << "Platform size 0";
            return false;
        }
        for (auto& platform : platforms)
        {
            d->printPlatformInfo(platform);
            d->platforms.push_back(platform);
        }
        std::vector<cl_context_properties> prop;
        // If we have a GL context, check which device is being used and use
        // that one.
        d->glCtxHnd = context->nativeGlContext();
        d->hdc = reinterpret_cast<HDC>(context->nativeDeviceHandle());
        if (d->glCtxHnd) 
        {
            d->devices.clear();
#ifdef __APPLE__
            prop.push_back(CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE);
            prop.push_back(d->glCtxHnd);
#endif
#ifdef _WIN32
            prop.push_back(CL_GL_CONTEXT_KHR);
            prop.push_back(d->glCtxHnd);
            prop.push_back(CL_WGL_HDC_KHR);
            prop.push_back(reinterpret_cast<cl_context_properties>(d->hdc));
#endif
            auto           glClDevices = devicesForGLContext();
            cl_device_type devType =
                d->useGpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU;
            TRACE(debug) << "Devices that can be shared with OpenGL:";
            for (auto& device : glClDevices)
            {
                d->printDeviceInfo(device);
                if (device.getInfo<CL_DEVICE_TYPE>() == devType)
                {
                    d->devices.push_back(device);
                    break;
                }
            }
            if (!d->devices.empty())
            {
                prop.push_back(CL_CONTEXT_PLATFORM);
                prop.push_back(
                    (cl_context_properties)d->devices[0].getInfo<CL_DEVICE_PLATFORM>());
            }
        }
        else
        {
            // serch for the first device of the desired kind.
            d->devices = d->devicesOfType(d->platforms,
                                          d->useGpu ? CL_DEVICE_TYPE_GPU
                                                    : CL_DEVICE_TYPE_CPU);
        }
        prop.push_back(0);
        if (d->devices.size() == 0)
        {
            TRACE(error) << "Failed to find devices";
            return false;
        }
        d->context =
            cl::Context(d->devices[0], prop.size() == 1 ? NULL : prop.data());
        d->queue = cl::CommandQueue(d->context, d->devices[0], 0, &err);
    }
    catch (cl::Error e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << errorToString(e.err()) << "): " << e.what();
    }

    d->initialized = true;
    return d->initialized;
}

/**
 * Returns the context created by the manager at initialization.
 */
cl::Context CLManager::context()
{
    if (!d->initialized)
    {
        throw std::runtime_error("CLManager not initialized");
    }
    return d->context;
}

/**
 * Returns the command queue created at intialization.
 */
cl::CommandQueue CLManager::commandQueue()
{
    if (!d->initialized)
    {
        throw std::runtime_error("CLManager not initialized");
    }
    return d->queue;
}

std::vector<cl::Platform> CLManager::platforms() const
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    return platforms;
}

std::vector<cl::Device> CLManager::devices() { return d->devices; }

void CLManager::Impl::printPlatformInfo(const cl::Platform& platform)
{
    std::string value;
    platform.getInfo(CL_PLATFORM_NAME, &value);
    TRACE(debug) << "Platform name: " << value;
    std::string extensions;
    platform.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
    TRACE(debug) << "Extensions: " << extensions;

    std::vector<cl::Device> devs;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    for (auto devIt = devs.begin(); devIt != devs.end(); ++devIt)
    {
        printDeviceInfo(*devIt);
    }
}

void CLManager::Impl::printDeviceInfo(const cl::Device& device)
{
    TRACE(debug) << "Name: " << device.getInfo<CL_DEVICE_NAME>();
    cl_device_type devtype = device.getInfo<CL_DEVICE_TYPE>();
    TRACE(debug) << "Type: " << devtype;
    TRACE(debug) << "CL_DEVICE_MAX_WORK_ITEM_SIZES: (";
    auto max_sizes = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
    for (auto i = max_sizes.begin(); i != max_sizes.end(); ++i)
    {
        TRACE(debug) << *(i) << " ";
    }
    TRACE(debug) << ")";

    TRACE(debug) << "CL_DEVICE_MAX_WORK_GROUP_SIZE: (";
    TRACE(debug) << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    TRACE(debug) << ")";
}

std::vector<cl::Device>
CLManager::Impl::devicesOfType(std::vector<cl::Platform>& platforms,
                               cl_device_type             type)
{
    std::vector<cl::Device> deviceList;

    // serch for the first device of the desired kind.
    for (auto& platform : platforms)
    {
        std::vector<cl::Device> list = devicesOfType(platform, type);
        deviceList.insert(deviceList.end(), list.begin(), list.end());
    }
    return deviceList;
}

std::vector<cl::Device> CLManager::Impl::devicesOfType(cl::Platform& platform,
                                                       cl_device_type type)
{
    std::vector<cl::Device> deviceList;

    try
    {
        std::vector<cl::Device> list;
        platform.getDevices(type, &list);
        deviceList.insert(deviceList.end(), list.begin(), list.end());
    }
    catch (cl::Error& e)
    {
        // ignore device not found error, rethrow all others
        if (e.err() != CL_DEVICE_NOT_FOUND)
        {
            throw;
        }
    }

    return deviceList;
}
