//
//  OCLManager.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opencl/OCLManager.h>


struct CLManager::Impl {
	cl::Context				context;
	cl::CommandQueue 		queue;
	std::vector<cl::Device> devices;
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

/**
 * Method used to initialize OpenCL. This creates a default context and
 * a command queue.
 */
bool CLManager::initialize()
{
	bool ret = true;
	cl_int err = CL_SUCCESS;
	
	try {
		std::cout << "Initializing OpenCL..." << std::endl;
		std::string value;
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.size() == 0) {
			std::cout << "Platform size 0\n";
			return -1;
		}
		
		platforms[0].getInfo(CL_PLATFORM_NAME, &value);
		std::cout << "Platform name: " << value << std::endl;
		
		cl_context_properties properties[] =
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
		d->context = cl::Context(CL_DEVICE_TYPE_CPU, properties);
		
		d->devices = d->context.getInfo<CL_CONTEXT_DEVICES>();
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
	return ret;
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