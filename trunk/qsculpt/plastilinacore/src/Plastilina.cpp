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


struct PlastilinaEngineState {
	bool openclInitialized;
	bool openglInitialized;
	
	PlastilinaEngineState()
	: openclInitialized(false),
	  openglInitialized(false)
	{
		
	}
};

PlastilinaEngineState g_engineState;

bool PlastilinaEngine::initialize(uint subsystem)
{
	if ((subsystem & PS_OPENCL)) {
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
