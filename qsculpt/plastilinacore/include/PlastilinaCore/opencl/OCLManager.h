//
//  OCLManager.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//

#ifndef __PlastilinaCore__opencl__OCLManager__
#define __PlastilinaCore__opencl__OCLManager__

#include <iostream>
#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>
#include "cl.hpp"

class CLManager {
	
public:
	static CLManager*	instance();
	
	/**
	 * Method used to initialize OpenCL. This creates a default context and
	 * a command queue.
	 */
	bool initialize();
	
	/**
	 * Method used to free all OpenCL objects. This will destroy all command
	 * queues and contexts managed by this manager object.
	 */
	void destroy();
	
	/**
	 * Returns the context created by the manager at initialization.
	 */
	cl::Context context();
	
	/**
	 * Returns the command queue created at intialization.
	 */
	cl::CommandQueue commandQueue();
	
	/**
	 *
	 */
	std::vector<cl::Device> devices();
	
	
private:
	CLManager();
	
	struct Impl;
	std::unique_ptr<Impl> d;
	
};


#endif /* defined(__PlastilinaCore__OCLManager__) */
