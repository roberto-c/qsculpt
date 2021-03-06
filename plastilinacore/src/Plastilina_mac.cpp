/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/IDevice.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Plastilina_priv.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Utilities.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/OpenGL.h>

#include <CoreFoundation/CoreFoundation.h>

 // forward declarations
void Plastilina_intializeOptions(const std::string& filepath);

intptr_t get_gl_context()
{
    // Get current CGL Context and CGL Share group
    CGLContextObj    kCGLContext    = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    return (intptr_t)kCGLShareGroup;
}

bool PlastilinaEngine::initialize(PlastilinaSubsystem subsystem)
{
    // Setup default search directories
    ResourcesManager::addResourcesDirectory(core::utils::get_app_path());
    ResourcesManager::addResourcesDirectory(".");

    auto platforms = core::getPlatformList();
    for (auto& platform : platforms)
    {
        auto devices = platform->deviceList();
        for (auto& device : devices)
        {
            TRACE(info) << "Vendor: " << device->vendor();
            TRACE(info) << "Name: " << device->name();
        }
    }

    if ((subsystem & PlastilinaSubsystem::OPENGL) !=
        PlastilinaSubsystem::NONE)
    {
        glewExperimental = TRUE;
        GLenum glewerror = glewInit();
        if (glewerror != GLEW_OK)
        {
            throw core::GlException("Failed to initialize GLEW", glewerror);
        }
        // clear glError g=flags
        glewerror = glGetError();
        if (glewerror != GL_NO_ERROR)
        {
            TRACE(debug) << "GLEW initialization succeded, but there is GL "
                "error code, ignoring...";
            while (glGetError() != GL_NO_ERROR)
                ;
        }
        g_engineState.openglInitialized = true;
    }

    if ((subsystem & PlastilinaSubsystem::OPENCL) !=
        PlastilinaSubsystem::NONE)
    {
        CLManager::startup(subsystem);
        if (((subsystem & PlastilinaSubsystem::ENABLE_CL_GL_SHARING) !=
            PlastilinaSubsystem::NONE) &&
            ((subsystem & PlastilinaSubsystem::OPENGL) !=
                PlastilinaSubsystem::NONE))
        {
            intptr_t glCtx = get_gl_context();
            CLManager::instance()->setOpenGLContext(glCtx);
        }

        g_engineState.openclInitialized =
            CLManager::instance()->initialize(subsystem);
        if (!g_engineState.openclInitialized)
        {
            std::cerr << "Failed to initialize OpenCL" << std::endl;
        }
    }
    return true;
}
