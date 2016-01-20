//
//  OCLManager.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>

#include <iostream>

struct CLManager::Impl {
    std::vector<cl::Platform> platforms;
	cl::Context				context;
	cl::CommandQueue 		queue;
	std::vector<cl::Device> devices;
	
	bool					useGpu;
	bool                    initialized;
    intptr_t				glCtxHnd;
#ifdef _WIN32
    HDC                     hdc;
#endif
	
	Impl() 
        : useGpu(false)
        , initialized(false)
        , glCtxHnd(0)
#ifdef _WIN32
        , hdc(0)
#endif
	{}

    void printPlatformInfo(const cl::Platform & platform);

    void printDeviceInfo(const cl::Device & device);
};

CLManager * g_clManager = NULL;

CLManager* CLManager::instance()
{
	if (!g_clManager) {
		g_clManager = new CLManager();
	}
	return g_clManager;
}

std::vector<cl::Device> CLManager::devicesForGLContext()
{
    std::vector<cl::Device> deviceList;

    if (!d->glCtxHnd)
    {
        TRACE(debug) << "No OpenGL context specified. Not using CL/GL sharing.";
        return deviceList;
    }

    std::vector<cl_context_properties> prop;
    for (auto & platform : d->platforms)
    {
        try
        {
            prop.clear();
            prop.push_back(CL_CONTEXT_PLATFORM);
            prop.push_back((cl_int)platform());
#ifdef __APPLE__
            prop.push_back(CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE);
            prop.push_back(d->glCtxHnd);
#endif
#ifdef _WIN32
            prop.push_back(CL_GL_CONTEXT_KHR);
            prop.push_back(d->glCtxHnd);
            prop.push_back(CL_WGL_HDC_KHR);
            prop.push_back(cl_int(d->hdc));
#endif
            prop.push_back(0);

            size_t bytes = 0;
            clGetGLContextInfoKHR_fn clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)platform.getExtensionFunctionAddress("clGetGLContextInfoKHR");
            if (!clGetGLContextInfoKHR)
            {
                TRACE(debug) << "Function pointer not found clGetGLContextInfoKHR";
                continue;
            }
            cl_int error = clGetGLContextInfoKHR(prop.data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &bytes);
            cl::detail::errHandler(error, "clGetGLContextInfoKHR");
            // allocating the mem
            size_t devNum = bytes / sizeof(cl_device_id);
            std::vector<cl_device_id> devs(devNum);
            //reading the info
            error = clGetGLContextInfoKHR(prop.data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, bytes, devs.data(), NULL);
            cl::detail::errHandler(error, "clGetGLContextInfoKHR");
            for (auto & dev : devs)
            {
                deviceList.push_back(cl::Device(dev));
            }
        }
        catch (cl::Error & e)
        {
            TRACE(error) << "Failed to get platform info";
            TRACE(error) << "OpenCL exception:" << e.err() << " (" << core::cl::errorToString(e.err()) << "): " << e.what();
            d->printPlatformInfo(platform);
        }
    }
    return deviceList;
}

bool CLManager::isExtensionSupported(const cl::Platform platform, const std::string & name) const
{
    std::string extensions;
    platform.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);

    return false;
}

CLManager::CLManager() : d(new Impl)
{
	
}

CLManager::~CLManager()
{
}

void CLManager::setUseGPU(bool useGPU)
{
	d->useGpu = useGPU;
}

void CLManager::setOpenGLContext(intptr_t hnd)
{
    d->glCtxHnd = hnd;
}

#ifdef _WIN32
void CLManager::setDeviceContext(HDC hdc)
{
    d->hdc = hdc;
}
#endif

/**
 * Method used to initialize OpenCL. This creates a default context and
 * a command queue.
 */
bool CLManager::initialize()
{
	cl_int err = CL_SUCCESS;
	
	if (d->initialized) {
		TRACE(debug) << "Already initialized.";
		return true;
	}
	try {
        TRACE(debug) << "Initializing OpenCL...";
		std::string value;
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.size() == 0) {
            TRACE(debug) << "Platform size 0";
			return false;
		}
        for (auto & platform : platforms)
        {
            d->printPlatformInfo(platform);
            d->platforms.push_back(platform);
        }
        std::vector<cl_context_properties> prop;
        // If we have a GL context, check which device is being used and use that one.
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
            prop.push_back(cl_int(d->hdc));
#endif
            auto glClDevices = devicesForGLContext();
            TRACE(debug) << "Devices that can be shared with OpenGL:";
            for (auto & device : glClDevices)
            {
                d->printDeviceInfo(device);
                d->devices.push_back(device);
            }
            if (!glClDevices.empty())
            {
                prop.push_back(CL_CONTEXT_PLATFORM);
                prop.push_back((cl_int)d->devices[0].getInfo<CL_DEVICE_PLATFORM>());
            }
        }
        prop.push_back(0);
		d->context = cl::Context(d->devices[0], prop.size() ==1 ? NULL : prop.data());
		d->queue = cl::CommandQueue(d->context, d->devices[0], 0, &err);
	}
	catch (cl::Error e) {
        TRACE(error) << "OpenCL exception:" << e.err() << " (" << core::cl::errorToString(e.err()) << "): " << e.what() << "\n";
	}
	
	d->initialized = true;
	return d->initialized;
}

bool CLManager::initializeWithGLContext(void * oglCtx, void * deviceCtx)
{
    return false;
}

/**
 * Method used to free all OpenCL objects. This will destroy all command
 * queues and contexts managed by this manager object.
 */
void CLManager::destroy()
{
	
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

std::vector<cl::Device> CLManager::devices()
{
	return d->devices;
}

void CLManager::Impl::printPlatformInfo(const cl::Platform & platform) 
{
    std::string value;
    platform.getInfo(CL_PLATFORM_NAME, &value);
    TRACE(debug) << "Platform name: " << value;
    std::string extensions;
    platform.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
    TRACE(debug) << "Extensions: " << extensions;

    std::vector<cl::Device> devs;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    for (auto devIt = devs.begin(); devIt != devs.end(); ++devIt) {
        printDeviceInfo(*devIt);
    }
}

void CLManager::Impl::printDeviceInfo(const cl::Device & device)
{
    TRACE(debug) << "Name: " << device.getInfo<CL_DEVICE_NAME>();
    cl_device_type devtype = device.getInfo<CL_DEVICE_TYPE>();
    TRACE(debug) << "Type: " << devtype;
    TRACE(debug) << "CL_DEVICE_MAX_WORK_ITEM_SIZES: (";
    auto max_sizes = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
    for (auto i = max_sizes.begin(); i != max_sizes.end(); ++i) {
        TRACE(debug) << *(i) << " ";
    }
    TRACE(debug) << ")";

    TRACE(debug) << "CL_DEVICE_MAX_WORK_GROUP_SIZE: (";
    TRACE(debug) << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    TRACE(debug) << ")";
}
