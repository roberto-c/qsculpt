//
//  OCLManager.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opencl/OCLManager.h>


#include <iostream>

struct CLManager::Impl {
	cl::Context				context;
	cl::CommandQueue 		queue;
	std::vector<cl::Device> devices;
	
	bool					useGpu;
	bool                    initialized;
    intptr_t				glCtxHnd;
	
	Impl() : useGpu(false), initialized(false), glCtxHnd(0)
	{}
};

CLManager * g_clManager = NULL;

CLManager* CLManager::instance()
{
	if (!g_clManager) {
		g_clManager = new CLManager();
	}
	return g_clManager;
}

CLManager::CLManager() : d(new Impl)
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

/**
 * Method used to initialize OpenCL. This creates a default context and
 * a command queue.
 */
bool CLManager::initialize()
{
	cl_int err = CL_SUCCESS;
	
	if (d->initialized) {
		std::cerr << "Already initialized.\n";
		return true;
	}
	try {
		std::cout << "Initializing OpenCL..." << std::endl;
		std::string value;
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.size() == 0) {
			std::cout << "Platform size 0\n";
			return false;
		}
		
		platforms[0].getInfo(CL_PLATFORM_NAME, &value);
		std::cout << "Platform name: " << value << std::endl;
        std::string extensions;
        platforms[0].getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
        std::cout << "Extensions: " << extensions << std::endl;
		
		cl_device_type deviceTypeToUse = CL_DEVICE_TYPE_CPU;
		if (d->useGpu) {
			deviceTypeToUse = CL_DEVICE_TYPE_GPU;
			std::cout << "Trying to use GPU device." << std::endl;
		} else {
			std::cout << "Trying to use CPU device." << std::endl;
		}
		
		std::vector<cl::Device> devices;
		platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
		for (auto dev = devices.begin(); dev != devices.end(); ++dev) {
			std::cout << "Name: " << (*dev).getInfo<CL_DEVICE_NAME>() << std::endl;
			cl_device_type devtype = (*dev).getInfo<CL_DEVICE_TYPE>();
			std::cout << "Type: " <<  devtype << std::endl;
			if (devtype == deviceTypeToUse) {
				d->devices.push_back(*dev);
			}
			{
				std::cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES: (";
				auto max_sizes = (*dev).getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
				for (auto i = max_sizes.begin(); i != max_sizes.end(); ++i) {
					std::cout << *(i) << " ";
				}
				std::cout << ")" << std::endl;
			}
			{
				std::cout << "CL_DEVICE_MAX_WORK_GROUP_SIZE: (";
				std::cout << (*dev).getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
				std::cout << ")" << std::endl;
			}
		}
        std::vector<cl_context_properties> prop;
        if (d->glCtxHnd) {
#ifdef __APPLE__
            prop.push_back(CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE);
            prop.push_back(d->glCtxHnd);
#endif
        }
        prop.push_back(0);
		d->context = cl::Context(d->devices, prop.size() ==1 ? NULL : prop.data());
		d->queue = cl::CommandQueue(d->context, d->devices[0], 0, &err);
	}
	catch (cl::Error err) {
		std::cerr
		<< "ERROR: "
		<< err.what()
		<< "("
		<< err.err()
		<< ")"
		<< std::endl;
	}
	
	d->initialized = true;
	return d->initialized;
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
	return d->context;
}

/**
 * Returns the command queue created at intialization.
 */
cl::CommandQueue CLManager::commandQueue()
{
	return d->queue;
}

std::vector<cl::Device> CLManager::devices()
{
	return d->devices;
}