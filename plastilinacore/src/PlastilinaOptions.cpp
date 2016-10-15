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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Utilities.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::options_description g_optionsDesc;
po::variables_map       g_options;

bool g_initialized = false;

void Plastilina_intializeOptions(const std::string & filepath)
{
    using namespace std;
    using namespace core::utils;
    vector<string> default_search_dirs = { get_app_path() };

    if (g_initialized) {
        return;
    }
    vector<string> defaultRendering = { string("OpenGL") };
    vector<string> defaultSubsystems = { string("Video") };
    g_optionsDesc.add_options()
        // PlastilinaCore section
        ("PlastilinaCore.RenderEngine", po::value<vector<string>>()->default_value(defaultRendering, "OpenGL"), "Render engine to use.")
        ("PlastilinaCore.EnableSubsystems", po::value<vector<string>>()->default_value(defaultSubsystems, "Video"), "Which subsystems to initalize. Video, Audio, Physics.")
        ("PlastilinaCore.ResourcesDir", po::value<vector<string>>()->default_value(default_search_dirs, get_app_path()), "path used to load all resources")
        ("PlastilinaCore.Verbosity", po::value<int>()->default_value(2), "verbosity level to print")
        // Vulkan section
        ;
    po::store(po::parse_config_file<char>(filepath.c_str(), g_optionsDesc), g_options);
    po::notify(g_options);
}

po::variables_map Plastilina_getOptions()
{
    return g_options;
}