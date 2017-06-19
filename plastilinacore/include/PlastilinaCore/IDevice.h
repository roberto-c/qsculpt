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

#include <PlastilinaCore/Variant.h>
#include <functional>
#include <string>

namespace core
{
enum class ApiSupported : unsigned int;
/**
 * Abstract class to identify
 */
class IDevice
{
  public:
    IDevice(){};

    virtual ~IDevice(){};

    virtual ApiSupported api() const = 0;

    virtual std::string vendor() const = 0;

    virtual std::string name() const = 0;

    virtual std::string driverString() const = 0;

    virtual Variant attribute(const std::string& name) const = 0;
};

typedef std::vector<std::unique_ptr<IDevice>> DeviceList;

class IPlatform
{
  public:
    typedef std::function<bool(const IDevice*)> DeviceFilter;

    virtual ~IPlatform(){};

    //virtual bool isSupported() const = 0;

    virtual core::DeviceList
    deviceList(DeviceFilter filter = DeviceFilter()) const = 0;
};

typedef std::vector<std::unique_ptr<IPlatform>> PlatformList;

core::PlatformList getPlatformList();
};