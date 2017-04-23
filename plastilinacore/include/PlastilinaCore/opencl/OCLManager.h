/***************************************************************************
 *   Copyright (C) 2012-2017 by Juan Roberto Cabral Flores                 *
 *   roberto.cabral@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#pragma once

#include <PlastilinaCore/opencl/OpenCL.h>

/**
 Class to initialize and manage all OpenCL resources.

 It is used to create a OpenCL context, setup attributes, specify shareing
 with OpenGL, etc.

 There is only one manager and it is usually intialized by the library
 subsystem intialization routine.
 */
class DLLEXPORT CLManager
{

  public:
    /**
     Returns the instance yo the OpenCL Manager.
     */
    static CLManager* instance();
    static bool
    startup(PlastilinaSubsystem flags = PlastilinaSubsystem::NONE);
    static bool shutdown();

    ~CLManager();

    /**
     Set to true to ask for an GPU device.
     This method should be called before initializing any OpenCL context
     (before calling any of the intialze methods.

     @TODO Remove function and replace a createContext method
    */
    void setUseGPU(bool useGPU);

    /**
     Set the OpenGL context to use to share resources with.
     */
    void setOpenGLContext(intptr_t hnd);

#ifdef _WIN32
    void setDeviceContext(HDC hdc);
#endif

    /**
     Create context using specified device. All future operations that ask for
     current context, will use the one created by the most recent call to any
     of the cretexContext* methods or the lasr call to setCurrentContext.
     */
    // cl::Context createContext(cl_device_type);
    // cl::Context createContextWithGLContext(void* oglCtx, void* deviceCtx);

    /**
     * Method used to initialize OpenCL. This creates a default context and
     * a command queue.
     */
    bool initialize(PlastilinaSubsystem flags = PlastilinaSubsystem::NONE);

    /**
     * Initialize OpenCL context with the use of OpenGL context specified.
     *
     */
    bool initializeWithGLContext(void* oglCtx, void* deviceCtx);

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
    void setCurrentContext(cl::Context& clctx);

    /**
     * Returns the command queue created at intialization.
     */
    cl::CommandQueue commandQueue();

    /**
     */
    std::vector<cl::Platform> platforms() const;

    /**
     *
     */
    std::vector<cl::Device> devices();

    /**
     * Returns a list of devices that can be shared with the current
     * OpenGL context
     */
    std::vector<cl::Device> devicesForGLContext();

    /**
     * Return s true if the extension is supported
     */
    bool isExtensionSupported(const cl::Platform platform,
                              const std::string& name) const;

  private:
    CLManager();

    struct Impl;
    std::unique_ptr<Impl> d;
};
