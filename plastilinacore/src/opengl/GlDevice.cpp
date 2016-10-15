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

#include <PlastilinaCore/opengl/GlDevice.h>
#include <PlastilinaCore/opengl/OpenGL.h>

#include <string>

namespace gl
{
GlDevice::GlDevice(std::string vendor,
        std::string name,
        std::string driverString)
    : vendor_(vendor)
    , name_(name)
    , driverString_(driverString)
{
    
}

GlDevice::~GlDevice()
{

}

core::ApiSupported GlDevice::api() const
{
    return core::ApiSupported::OPENGL;
}

std::string GlDevice::vendor() const
{
    return vendor_;
}

std::string GlDevice::name() const
{
    return name_;
}

std::string GlDevice::driverString() const
{
    return driverString_;
}

core::Variant GlDevice::attribute(const std::string & name) const
{
    return "";
}

GlPlatform::GlPlatform()
{
}

GlPlatform::~GlPlatform()
{
}

core::DeviceList GlPlatform::deviceList(DeviceFilter filter) const
{
    using namespace std;

    auto vendorStr = glGetString(GL_VENDOR);
    auto rendererStr = glGetString(GL_RENDERER);
    unique_ptr<core::IDevice> device = make_unique<GlDevice>(
        string((const char*)vendorStr),
        string((const char*)rendererStr));
    auto list = core::DeviceList();
    list.push_back(std::move(device));
    return list;
}

bool GlPlatform::isSupported() const
{
    try
    {
        return true;
    }
    catch (...)
    {

    }
    return false;
}

};
