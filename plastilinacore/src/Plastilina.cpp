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
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Utilities.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#if HAS_VULKAN
#include <PlastilinaCore/vulkan/VkUtils.h>
#include <PlastilinaCore/vulkan/Vulkan.h>
#endif

#include <boost/program_options.hpp>

// forward declarations
void Plastilina_intializeOptions(const std::string& filepath);

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace po = boost::program_options;

struct PlastilinaEngineState
{
    po::options_description optionsDesc;
    po::variables_map       options;

    std::shared_ptr<core::Context> defaultctx;
    std::shared_ptr<core::Context> currentctx;
    bool                           openclInitialized;
    bool                           openglInitialized;
    std::string                    resourcesPath;

    PlastilinaEngineState()
        : openclInitialized(false)
        , openglInitialized(false)
    {
        config_setup();
    }

    void config_setup()
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
};

PlastilinaEngineState g_engineState;

#ifdef __APPLE__
intptr_t get_gl_context()
{
    // Get current CGL Context and CGL Share group
    CGLContextObj    kCGLContext    = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    return (intptr_t)kCGLShareGroup;
}
#elif defined(_WIN32)
intptr_t get_gl_context()
{
    // Get current CGL Context and CGL Share group
    return (intptr_t)wglGetCurrentContext();
}
HDC get_device_context() { return wglGetCurrentDC(); }
#else
intptr_t get_gl_context()
{
    std::cout << "Unknow system. Don't know how to get the window handle"
              << std::endl;
    return NULL;
}
#endif

bool PlastilinaEngine::initialize(PlastilinaSubsystem subsystem)
{
    // Setup default search directories
    ResourcesManager::setResourcesDirectory(core::utils::get_app_path());
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
#ifdef HAS_VULKAN
    else if ((subsystem & PlastilinaSubsystem::VULKAN) !=
             PlastilinaSubsystem::NONE)
    {
        if (!vulkan::isVulkanSupported())
        {
            TRACE(error) << "Vulkan is not supported! Install new drivers or "
                            "use another rendering API.";
        }
    }
#endif
    if ((subsystem & PlastilinaSubsystem::OPENCL) !=
        PlastilinaSubsystem::NONE)
    {
        CLManager::startup(subsystem);
#ifdef _WIN32
        if (((subsystem & PlastilinaSubsystem::ENABLE_CL_GL_SHARING) !=
             PlastilinaSubsystem::NONE) &&
            ((subsystem & PlastilinaSubsystem::OPENGL) !=
             PlastilinaSubsystem::NONE))
        {
            intptr_t glCtx = get_gl_context();
            CLManager::instance()->setOpenGLContext(glCtx);
            CLManager::instance()->setDeviceContext(get_device_context());
        }
#endif

        g_engineState.openclInitialized =
            CLManager::instance()->initialize(subsystem);
        if (!g_engineState.openclInitialized)
        {
            std::cerr << "Failed to initialize OpenCL" << std::endl;
        }
    }
    return true;
}

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
