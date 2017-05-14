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
#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/IDevice.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Plastilina_priv.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Utilities.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#ifdef HAS_VULKAN
#include <PlastilinaCore/vulkan/VkUtils.h>
#include <PlastilinaCore/vulkan/Vulkan.h>
#endif


// forward declarations
void Plastilina_intializeOptions(const std::string& filepath);


namespace po = boost::program_options;

PlastilinaEngineState::PlastilinaEngineState()
    : openclInitialized(false)
    , openglInitialized(false)
{
    config_setup();
}

void PlastilinaEngineState::config_setup()
{
    using namespace std;
    using namespace core::utils;
    vector<string> default_search_dirs = {get_app_path()};

    optionsDesc.add_options()("resourcesdir",
                              po::value<vector<string>>()->default_value(
                                  default_search_dirs, get_app_path()),
                              "path used to load all resources")(
        "verbosity",
        po::value<boost::log::trivial::severity_level>()->default_value(
            boost::log::trivial::info),
        "verbosity level to print");
}

PlastilinaEngineState g_engineState;

bool PlastilinaEngine::initializeWithAttributes(AttributeMap attr)
{
    return false;
}

bool PlastilinaEngine::initializeFromCommandLine(int argc, const char** athv)
{
    return false;
}

bool PlastilinaEngine::initializeFromConfigFile(const std::string& filepath)
{
    try
    {
        Plastilina_intializeOptions(filepath);
        // if (g_engineState.options.count("verbosity") > 0) {
        //    TRACE(info) << "verbosity: " <<
        //    g_engineState.options["verbosity"].as<boost::log::trivial::severity_level>();
        //}
        for (auto it : g_engineState.options)
        {
            TRACE(info) << "Name: " << it.first;
        }

        return true;
    }
    catch (std::exception& e)
    {
        TRACE(error) << "Failed to read config file. " << e.what();
    }

    return false;
}

bool PlastilinaEngine::shutdown() { return true; }

void PlastilinaEngine::setCurrentContext(std::shared_ptr<core::Context>& ctx)
{
    g_engineState.currentctx = ctx;
}

core::Context& PlastilinaEngine::currentContext()
{
    if (!g_engineState.currentctx)
    {
        throw std::runtime_error("No current context set");
    }
    return *g_engineState.currentctx;
}
