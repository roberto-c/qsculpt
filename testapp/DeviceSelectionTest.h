/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
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

#include "Stable.h"
#include <PlastilinaCore/IDevice.h>
#include <PlastilinaCore/opengl/GlDevice.h>
#include <PlastilinaCore/vulkan/VkDevice.h>
#include "BaseTest.h"

class DeviceSelectionTest : public BaseTest
{

  public:
    DeviceSelectionTest();

    virtual ~DeviceSelectionTest();

  protected:
    virtual void doSetup();

    virtual void doRun();

    virtual void doShutdown();

  private:
    Eigen::IOFormat octaveFmt;
};

inline DeviceSelectionTest::DeviceSelectionTest()
    : BaseTest("DeviceSelectionTest")
    , octaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]")
{
}

inline DeviceSelectionTest::~DeviceSelectionTest() {}

inline void DeviceSelectionTest::doSetup() 
{
    TRACE(info) << "setup";
}

inline void DeviceSelectionTest::doRun()
{
    TRACE(info) << "run";
    auto platforms = core::getPlatformList();
    for (auto& platform : platforms)
    {
        if (!platform)
        {
            continue;
        }
        auto devices = platform->deviceList();
        for (auto& device : devices)
        {
            if (!device)
            {
                continue;
            }
            TRACE(info) << "Device API type:" << (uint32_t)device->api();
            TRACE(info) << "Device name:" << device->name();
            TRACE(info) << "Device vendor:" << device->vendor();
            TRACE(info) << "Device driver string:" << device->driverString();
        }
    }
}

inline void DeviceSelectionTest::doShutdown() 
{ 
    TRACE(info) << "shutdown"; 
}
