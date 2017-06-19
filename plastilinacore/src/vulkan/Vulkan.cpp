/***************************************************************************
 *   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Version.h>
#include <PlastilinaCore/vulkan/Vulkan.h>

vk::Instance gVkInstance;

union vkVersion {
    uint32_t raw;
    struct
    {
        uint32_t patch : 12;
        uint32_t minor : 10;
        uint32_t major : 10;
    } fields;
};

namespace vulkan
{
vk::Instance getVkAppInstance()
{
    if (gVkInstance)
    {
        return gVkInstance;
    }
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo    appInfo;
    createInfo.pApplicationInfo = &appInfo;
    vkVersion version;
    version.fields.major       = 1;
    version.fields.minor       = 0;
    version.fields.patch       = 16;
    appInfo.apiVersion         = version.raw;
    appInfo.applicationVersion = 0x01000000;
    appInfo.engineVersion      = ENGINE_VERSION;
    appInfo.pApplicationName   = u8"Unknown App";
    std::vector<const char*> enabledExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME };
        // Enable surface extensions depending on os
#if defined(_WIN32)
        enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
        enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
        enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
    if (enabledExtensions.size() > 0)
    {
        createInfo.enabledExtensionCount =
            (uint32_t)enabledExtensions.size();
        createInfo.ppEnabledExtensionNames =
            enabledExtensions.data();
    }
    gVkInstance                = vk::createInstance(createInfo);
    return gVkInstance;
}

}
