//
//  CLRender.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 7/26/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//
#include "Stable.h"
#include "CLRender.h"

#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/opencl/OCLManager.h>

#include <string>

struct CLRender::Impl
{
    std::vector<unsigned char> buffer, bufferOut;
    gl::Texture2D::shared_ptr  glTexSrc, glTexDest;
    cl::Buffer                 imageInp, imageOut;
    cl::ImageGL                imageSrc, imageDest;
    cl::Program                program;
    cl::Kernel                 krnFilterImg;
    float                      time;
    bool                       validState;

    Impl()
        : time(0)
        , validState(true)
    {
    }
};

CLRender::CLRender()
    : d(new Impl)
{
    d->buffer.resize(1024 * 768 * 4, 0);
    d->bufferOut.resize(1024 * 768 * 4, 0);
}

CLRender::~CLRender()
{
    delete d;
    d = NULL;
}

void CLRender::initialize()
{
    if (!CLManager::instance()->initialize())
    {
        std::cerr << "Error: Failed to initialize OpenCL\n";
        throw std::runtime_error("Failed to initialize OpenCL");
    }
    CLManager* mngr = CLManager::instance();

    try
    {
        cl::ImageFormat format;
        format.image_channel_order     = CL_RGBA;
        format.image_channel_data_type = CL_UNSIGNED_INT8;

        d->imageInp = cl::Buffer(mngr->context(),
                                 CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                 d->buffer.size(), d->buffer.data());
        d->imageOut = cl::Buffer(mngr->context(),
                                 CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                 d->bufferOut.size(), d->bufferOut.data());

        ResourcesManager     mgr;
        std::string          path = mgr.findResourcePath("Render", "cl");
        std::string          kernelSource = core::cl::loadFromFile(path);
        d->program = cl::Program(mngr->context(), kernelSource);
        d->program.build();
        d->krnFilterImg = cl::Kernel(d->program, "filter_img");
    }
    catch (cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << core::cl::errorToString(e.err()) << "): " << e.what()
                     << "\n";
        if (e.err() == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string buildlog;
            d->program.getBuildInfo(mngr->devices()[0], CL_PROGRAM_BUILD_LOG,
                                    &buildlog);
            TRACE(error) << "Build log: " << buildlog << std::endl;
        }
    }
}

void CLRender::shutdown() {}

std::vector<unsigned char>& CLRender::buffer() { return d->buffer; }

const std::vector<unsigned char>& CLRender::buffer() const
{
    return d->buffer;
}

std::vector<unsigned char>& CLRender::bufferOutput() { return d->bufferOut; }

const std::vector<unsigned char>& CLRender::bufferOutput() const
{
    return d->bufferOut;
}

void CLRender::setGLTexSrc(gl::Texture2D::shared_ptr tex)
{
    try
    {
        d->glTexSrc    = tex;
        CLManager* mgr = CLManager::instance();
        d->imageSrc =
            cl::ImageGL(mgr->context(), 0, tex->target(), 0, tex->oid());
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << core::cl::errorToString(e.err()) << "): " << e.what();
        d->validState = false;
    }
}
gl::Texture2D::shared_ptr CLRender::glTexSrc() { return d->glTexSrc; }

void CLRender::setGLTexDest(gl::Texture2D::shared_ptr tex)
{
    try
    {
        d->glTexDest   = tex;
        CLManager* mgr = CLManager::instance();
        d->imageDest =
            cl::ImageGL(mgr->context(), 0, tex->target(), 0, tex->oid());
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << core::cl::errorToString(e.err()) << "): " << e.what();
        d->validState = false;
    }
}
gl::Texture2D::shared_ptr CLRender::glTexDest() { return d->glTexDest; }

void CLRender::swapBuffers()
{
    std::swap(d->bufferOut, d->buffer);

    std::swap(d->glTexDest, d->glTexSrc);
    std::swap(d->imageDest, d->imageSrc);
}

void CLRender::render(float step)
{
    if (!d->validState)
    {
        return;
    }
    try
    {
        d->time += step;
        CLManager*              clmgr = CLManager::instance();
        std::vector<cl::Memory> obj;
        obj.push_back(d->imageDest);
        obj.push_back(d->imageSrc);
        clmgr->commandQueue().enqueueAcquireGLObjects(&obj);
        d->krnFilterImg.setArg(0, d->imageSrc);
        d->krnFilterImg.setArg(1, d->imageDest);
        d->krnFilterImg.setArg(2, d->time);
        clmgr->commandQueue().enqueueNDRangeKernel(
            d->krnFilterImg, cl::NullRange, cl::NDRange(256, 256));
        clmgr->commandQueue().enqueueReleaseGLObjects(&obj);
        clmgr->commandQueue().flush();
    }
    catch (cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << core::cl::errorToString(e.err()) << "): " << e.what();
    }
}
