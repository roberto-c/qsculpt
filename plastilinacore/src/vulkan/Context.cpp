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

#include <PlastilinaCore/vulkan/Context.h>
#include <PlastilinaCore/vulkan/VkUtils.h>
#include <PlastilinaCore/vulkan/Vulkan.h>

namespace vulkan
{

struct Context::Impl
{
    VkCtxFlags        flags;
    vk::Instance      instance;
    vk::Device        device;
    vk::PipelineCache pipelineCache;
    vk::Queue         queue;
    uint32_t          graphicsQueueIndex;

    Impl(VkCtxFlags flags)
        : flags(flags)
    {
        vk::InstanceCreateInfo instanceCreateInfo;
    }

    uint32_t
    findQueue(const vk::PhysicalDevice& physicalDevice,
              const vk::QueueFlags&     flags,
              const vk::SurfaceKHR& presentSurface = vk::SurfaceKHR()) const
    {
        std::vector<vk::QueueFamilyProperties> queueProps =
            physicalDevice.getQueueFamilyProperties();
        size_t queueCount = queueProps.size();
        for (uint32_t i = 0; i < queueCount; i++)
        {
            if (queueProps[i].queueFlags & flags)
            {
                if (presentSurface &&
                    !physicalDevice.getSurfaceSupportKHR(i, presentSurface))
                {
                    continue;
                }
                return i;
            }
        }
        throw std::runtime_error("No queue matches the flags " +
                                 vk::to_string(flags));
    }
};

Context::Context(VkCtxFlags flags)
    : d(new Impl(flags))
{
    bool enableValidation =
        static_cast<uint32_t>(flags) &
        static_cast<uint32_t>(VkCtxFlags::EnableValidation);
    {
        // Vulkan instance
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName = "VulkanExamples";
        appInfo.pEngineName      = "VulkanExamples";
        appInfo.apiVersion       = VK_API_VERSION_1_0;

        std::vector<const char*> enabledExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME};
// Enable surface extensions depending on os
#if defined(_WIN32)
        enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
        enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
        enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

        vk::InstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        if (enabledExtensions.size() > 0)
        {
            if (enableValidation)
            {
                enabledExtensions.push_back(
                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            }
            instanceCreateInfo.enabledExtensionCount =
                (uint32_t)enabledExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames =
                enabledExtensions.data();
        }
        // if (enableValidation) {
        //    instanceCreateInfo.enabledLayerCount =
        //    debug::validationLayerCount;
        //    instanceCreateInfo.ppEnabledLayerNames =
        //    debug::validationLayerNames;
        //}
        d->instance = vk::createInstance(instanceCreateInfo);
    }

#if defined(__ANDROID__)
    loadVulkanFunctions(instance);
#endif

    // Physical device
    auto physicalDevices = d->instance.enumeratePhysicalDevices();
    // Note :
    // This example will always use the first physical device reported,
    // change the vector index if you have multiple Vulkan devices installed
    // and want to use another one
    auto physicalDevice = physicalDevices[0];
    struct Version
    {
        uint32_t patch : 12;
        uint32_t minor : 10;
        uint32_t major : 10;
    } _version;
    // Store properties (including limits) and features of the phyiscal device
    // So examples can check against them and see if a feature is actually
    // supported
    auto deviceProperties = physicalDevice.getProperties();
    memcpy(&_version, &deviceProperties.apiVersion, sizeof(uint32_t));
    auto deviceFeatures = physicalDevice.getFeatures();
    // Gather physical device memory properties
    auto deviceMemoryProperties = physicalDevice.getMemoryProperties();

    // Vulkan device
    {
        // Find a queue that supports graphics operations
        uint32_t graphicsQueueIndex =
            d->findQueue(physicalDevice, vk::QueueFlagBits::eGraphics);
        std::array<float, 1> queuePriorities = {0.0f};
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex           = graphicsQueueIndex;
        queueCreateInfo.queueCount                 = 1;
        queueCreateInfo.pQueuePriorities           = queuePriorities.data();
        std::vector<const char*> enabledExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos    = &queueCreateInfo;
        deviceCreateInfo.pEnabledFeatures     = &deviceFeatures;
        // enable the debug marker extension if it is present (likely meaning
        // a debugging tool is present)
        if (checkDeviceExtensionPresent(physicalDevice,
                                        VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
        {
            enabledExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
            // enableDebugMarkers = true;
        }
        if (enabledExtensions.size() > 0)
        {
            deviceCreateInfo.enabledExtensionCount =
                (uint32_t)enabledExtensions.size();
            deviceCreateInfo.ppEnabledExtensionNames =
                enabledExtensions.data();
        }
        if (enableValidation)
        {
            // deviceCreateInfo.enabledLayerCount =
            // debug::validationLayerCount;
            // deviceCreateInfo.ppEnabledLayerNames =
            // debug::validationLayerNames;
        }
        d->device = physicalDevice.createDevice(deviceCreateInfo);
    }

    if (enableValidation)
    {
        // debug::setupDebugging(d->instance,
        // vk::DebugReportFlagBitsEXT::eError |
        // vk::DebugReportFlagBitsEXT::eWarning);
    }

    // if (enableDebugMarkers) {
    //    debug::marker::setup(device);
    //}
    d->pipelineCache =
        d->device.createPipelineCache(vk::PipelineCacheCreateInfo());
    // Find a queue that supports graphics operations
    d->graphicsQueueIndex =
        d->findQueue(physicalDevice, vk::QueueFlagBits::eGraphics);
    // Get the graphics queue
    d->queue = d->device.getQueue(d->graphicsQueueIndex, 0);
}

Context::~Context() {}

uint32_t Context::windowHandle() { return 0; }

void Context::setWindowHandle(uint32_t windowHandle) {}
};
