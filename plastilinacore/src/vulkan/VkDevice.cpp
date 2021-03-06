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

#include <PlastilinaCore/vulkan/VkDevice.h>
#include <PlastilinaCore/vulkan/Vulkan.h>

#include <sstream>

namespace vulkan
{
Device::Device(std::string vendor, std::string name, std::string driverString)
    : vendor_(vendor)
    , name_(name)
    , driverString_(driverString)
{
}

Device::~Device() {}

core::ApiSupported Device::api() const { return core::ApiSupported::VULKAN; }

std::string Device::vendor() const { return vendor_; }

std::string Device::name() const { return name_; }

std::string Device::driverString() const { return driverString_; }

core::Variant Device::attribute(const std::string& name) const
{
    return core::Variant();
}

VkPlatform::VkPlatform() {}

VkPlatform::~VkPlatform() {}

bool VkPlatform::isSupported()
{
    try
    {
        auto instance = vulkan::getVkAppInstance();

        return true;
    }
    catch (...)
    {
        TRACE(info) << "VkPlatform::isSupported: Failed to detect vulkan";
        return false;
    }
    return false;
}

core::DeviceList VkPlatform::deviceList(DeviceFilter filter) const
{
    using namespace std;

    auto list     = core::DeviceList();
    try
    {
        auto instance = vulkan::getVkAppInstance();
        if (instance)
        {
            auto devices = instance.enumeratePhysicalDevices();
            for (auto& device : devices)
            {
                auto         props = device.getProperties();
                stringstream vendor;
                vendor << hex << props.vendorID << dec;
                unique_ptr<core::IDevice> dev = make_unique<Device>(
                    vendor.str(), string((const char*)props.deviceName));
                list.push_back(std::move(dev));
            }
        }
    }
    catch (const std::exception& e)
    {
        TRACE(error) << e.what();
    }
    return list;
}
}
