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
#pragma once

#include <PlastilinaCore/IDevice.h>
#include <PlastilinaCore/Variant.h>

#include <memory>
#include <string>
#include <vector>

class vk::Device;

namespace vulkan
{

    class VkDevice : public core::IDevice
    {
    public:
        VkDevice(std::string vendor = "Unknown",
            std::string name = "Unknown",
            std::string driverString = "Unknown");

        virtual ~VkDevice();

        // Inherited via IDevice
        virtual core::ApiSupported api() const override;
        virtual std::string vendor() const override;
        virtual std::string name() const override;
        virtual std::string driverString() const override;
        virtual core::Variant attribute(const std::string & name) const override;

    private:
        std::string     vendor_;
        std::string     name_;
        std::string     driverString_;
        vk::Device      device_;
    };

    typedef std::vector<std::unique_ptr<VkDevice>> VkDeviceList;

    class VkPlatform : public core::IPlatform
    {
    public:
        VkPlatform();

        virtual ~VkPlatform();

        // Inherited via IPlatform
        virtual bool isSupported() const override;

        virtual core::DeviceList deviceList(DeviceFilter filter = DeviceFilter()) const override;

    };

};