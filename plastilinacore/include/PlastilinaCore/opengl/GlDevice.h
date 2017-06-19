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

namespace gl
{

/**
 * Abstract class to identify
 */
class Device : public core::IDevice
{
  public:
    Device(std::string vendor = "Unknown", std::string name = "Unknown",
             std::string driverString = "Unknown");

    virtual ~Device();

    virtual core::ApiSupported api() const;

    virtual std::string vendor() const;

    virtual std::string name() const;

    virtual std::string driverString() const;

    virtual core::Variant attribute(const std::string& name) const;

  private:
    std::string vendor_;
    std::string name_;
    std::string driverString_;
};

typedef std::vector<std::unique_ptr<Device>> GlDeviceList;

class GlPlatform : public core::IPlatform
{
  public:
    static bool isSupported();

    GlPlatform();

    virtual ~GlPlatform();

    // Inherited via IPlatform
    virtual core::DeviceList
    deviceList(DeviceFilter filter = DeviceFilter()) const override;
};
};