//
//  OCLManager.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/8/12.
//
//

#ifndef __PlastilinaCore__opencl__OCLManager__
#define __PlastilinaCore__opencl__OCLManager__

#include <PlastilinaCore/opencl/OpenCL.h>

class DLLEXPORT CLManager {
	
public:
	static CLManager*	instance();

    ~CLManager();
	
	void setUseGPU(bool useGPU);
    
    void setOpenGLContext(intptr_t hnd);
    
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
     * Sets the current OpenCL context.
     */
    void setCurrentContext(cl::Context & clctx);
	
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
