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
#include <PlastilinaCore/IDevice.h>

#include <PlastilinaCore/opengl/GlDevice.h>

#ifdef HAS_VULKAN
#include <PlastilinaCore/vulkan/VkDevice.h>
#include <PlastilinaCore/vulkan/Vulkan.h>
#endif

namespace core
{

core::PlatformList getPlatformList()
{
    using namespace std;

    auto list = core::PlatformList();

    unique_ptr<IPlatform> platform = make_unique<gl::GlPlatform>();
    if (platform->isSupported())
    {
        list.push_back(move(platform));
    }

#ifdef HAS_VULKAN
    platform = make_unique<vulkan::VkPlatform>();
    if (platform->isSupported())
    {
        list.push_back(move(platform));
    }
#endif

    return list;
}
};
