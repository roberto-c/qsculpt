/***************************************************************************
 *   Copyright (C) 2013 by Juan Roberto Cabral Flores                      *
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

#include <PlastilinaCore/Plastilina.h>

#include <PlastilinaCore/opencl/OCLManager.h>

#include <CoreFoundation/CoreFoundation.h>

struct PlastilinaEngineState {
	bool openclInitialized;
	bool openglInitialized;
	std::string resourcesPath;
	
	PlastilinaEngineState()
	: openclInitialized(false),
	  openglInitialized(false)
	{
		
	}
};

PlastilinaEngineState g_engineState;

#ifdef __APPLE__
intptr_t get_window_handle() {
    // Get current CGL Context and CGL Share group
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
	return (intptr_t)kCGLShareGroup;
}
#else
intptr_t get_window_handle() {
    std::cout << "Unknow system. Don't know how to get the window handle" << std::endl;
	return NULL;
}
#endif


bool PlastilinaEngine::initialize(PlastilinaSubsystem subsystem)
{
	if ( (subsystem & PlastilinaSubsystem::OPENCL) != PlastilinaSubsystem::NONE) {
        if ((subsystem & PlastilinaSubsystem::ENABLE_CL_GL_SHARING) != PlastilinaSubsystem::NONE) {
            intptr_t sharegrp = get_window_handle();
            CLManager::instance()->setOpenGLContext(sharegrp);
        }
        
		g_engineState.openclInitialized = CLManager::instance()->initialize();
		if (!g_engineState.openclInitialized) {
			std::cerr << "Failed to initialize OpenCL" << std::endl;
		}
	}
	
	return true;
}

bool PlastilinaEngine::shutdown()
{
	return true;
}

void PlastilinaEngine::setResourcesFolder(const std::string & path)
{
	g_engineState.resourcesPath = path;
}

std::string PlastilinaEngine::resourcesFolder()
{
	return g_engineState.resourcesPath;
}