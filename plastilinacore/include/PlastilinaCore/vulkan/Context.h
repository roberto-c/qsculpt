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

#include <vector>
#include <PlastilinaCore/vulkan/Vulkan.h>

namespace vulkan
{
class Context
{
  public:
    enum class VkCtxFlags : uint32_t
    {
        None             = 0,
        EnableValidation = 1,
        EnableDebug      = 2,

    };

    Context(VkCtxFlags flags = VkCtxFlags::None);

    ~Context();

    uint32_t windowHandle();

    void setWindowHandle(uint32_t handle);

    std::vector<vk::Device> deviceList() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
};