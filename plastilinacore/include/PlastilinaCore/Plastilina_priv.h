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
#pragma once

#include <boost/program_options.hpp>

struct PlastilinaEngineState
{
    boost::program_options::options_description optionsDesc;
    boost::program_options::variables_map       options;

    std::shared_ptr<core::Context> defaultctx;
    std::shared_ptr<core::Context> currentctx;
    bool                           openclInitialized;
    bool                           openglInitialized;
    std::string                    resourcesPath;
    
    PlastilinaEngineState();
    void config_setup();
};

extern PlastilinaEngineState* g_engineState;