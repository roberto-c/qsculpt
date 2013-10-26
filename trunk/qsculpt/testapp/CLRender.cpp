//
//  CLRender.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 7/26/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#include "CLRender.h"

#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>

#include <string>

struct CLRender::Impl
{
	std::vector<unsigned char> buffer, bufferOut;
	cl::Buffer   imageInp, imageOut;
	cl::Program  program;
	cl::Kernel   kernel;
	
//	Impl() {
//		
//	}
};

CLRender::CLRender()
: d(new Impl)
{
	d->buffer.resize(1024*768*4, 0);
	d->bufferOut.resize(1024*768*4, 0);
}

CLRender::~CLRender()
{
	delete d;
	d = NULL;
}

void CLRender::initialize()
{
	if (!CLManager::instance()->initialize() ) {
		std::cerr << "Error: Failed to initialize OpenCL\n";
		throw std::runtime_error("Failed to initialize OpenCL");
	}
	CLManager * mngr = CLManager::instance();
	
	try {
		cl::ImageFormat format;
		format.image_channel_order = CL_RGBA;
		format.image_channel_data_type = CL_UNSIGNED_INT8;
		
		d->imageInp = cl::Buffer(mngr->context(),CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, d->buffer.size(), d->buffer.data());
		d->imageOut = cl::Buffer(mngr->context(),CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, d->bufferOut.size(), d->bufferOut.data());
		
		std::string kernelSource = opencl::loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/testapp/render.cl");
		cl::Program::Sources source(1,
									std::make_pair(kernelSource.c_str(),kernelSource.length()));
		d->program = cl::Program(mngr->context(), source);
		d->program.build();
		d->kernel = cl::Kernel(d->program,"filter");
	} catch (cl::Error & e) {
		std::cerr << "OpenCL exception:" << e.err() << ": " << e.what() << "\n";
	}
}

void CLRender::shutdown()
{
	
}

std::vector<unsigned char> & CLRender::buffer()
{
	return d->buffer;
}

const std::vector<unsigned char> & CLRender::buffer() const
{
	return d->buffer;
}

std::vector<unsigned char> & CLRender::bufferOutput()
{
	return d->bufferOut;
}

const std::vector<unsigned char> & CLRender::bufferOutput() const
{
	return d->bufferOut;
}

void CLRender::swapBuffers()
{
	std::swap(d->bufferOut, d->buffer);
}

void CLRender::render(float step)
{
	try {
		CLManager * clmgr = CLManager::instance();
		clmgr->commandQueue().enqueueWriteBuffer(d->imageInp, CL_TRUE, 0,
												 d->buffer.size(),
												 d->buffer.data());
		d->kernel.setArg(0, d->imageInp);
		d->kernel.setArg(1, d->imageOut);
		d->kernel.setArg(2, 1024);
		d->kernel.setArg(3, 768);
		clmgr->commandQueue().enqueueNDRangeKernel(d->kernel, cl::NullRange,
												   cl::NDRange(1024,768));
		clmgr->commandQueue().enqueueReadBuffer(d->imageOut, CL_TRUE, 0,
												d->bufferOut.size(),
												d->bufferOut.data());
	} catch (cl::Error & e) {
		std::cerr << "OpenCL exception:" << e.err() << ": " << e.what() << "\n";
	}
}
