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

    vector<string> defaultRendering  = {string("OpenGL")};
    vector<string> defaultSubsystems = {string("Video")};
    optionsDesc.add_options()
        // PlastilinaCore section
        ("PlastilinaCore.RenderEngine",
         po::value<vector<string>>()->default_value(defaultRendering,
                                                    "OpenGL"),
         "Render engine to use.")(
            "PlastilinaCore.EnableSubsystems",
            po::value<vector<string>>()->default_value(defaultSubsystems,
                                                       "Video"),
            "Which subsystems to initalize. Video, Audio, Physics.")(
            "PlastilinaCore.ResourcesDir",
            po::value<vector<string>>()->default_value(default_search_dirs,
                                                       get_app_path()),
            "path used to load all resources")(
            "PlastilinaCore.Verbosity", po::value<int>()->default_value(2),
            "verbosity level to print")
        // Vulkan section
        ;
}

PlastilinaEngineState g_engineState;

bool PlastilinaEngine::initializeWithAttributes(AttributeMap attr)
{
    return false;
}

bool PlastilinaEngine::initializeFromCommandLine(int argc, char** argv)
{
    try
    {
        po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(g_engineState.optionsDesc).allow_unregistered().run();
        po::store(parsed, g_engineState.options);
        po::notify(g_engineState.options);
        return true;
    }
    catch (std::exception& e)
    {
        TRACE(error) << "Failed to parse options: " << e.what();
    }
    return false;
}

bool PlastilinaEngine::initializeFromConfigFile(const std::string& filepath)
{
    try
    {
        po::store(po::parse_config_file<char>(filepath.c_str(), g_engineState.optionsDesc),
                  g_engineState.options);
        po::notify(g_engineState.options);
        return true;
    }
    catch (std::exception& e)
    {
        TRACE(error) << "Failed to read config file. " << e.what();
    }

    return false;
}

bool PlastilinaEngine::shutdown() 
{ 
    return true;
}

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

core::variables_map PlastilinaEngine::options()
{
    return g_engineState.options;
}