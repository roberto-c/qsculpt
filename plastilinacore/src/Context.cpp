/***************************************************************************
 *   Copyright (C) 2014 by Juan R Cabral                                   *
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
#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/opengl/Context.h>

#ifdef HAS_VULKAN
#include <PlastilinaCore/vulkan/Context.h>
#endif

namespace core
{

IGraphicsContext * IGraphicsContext::createGraphicsContext(GraphicsContextCreateInfo & createInfo)
{
    switch (createInfo.contextType)
    {
    case core::ContextType::OpenGL:
        {
            gl::Context * ctx = new gl::Context(createInfo);
            return ctx;
        }
    }
    return nullptr;
}

void IGraphicsContext::destroyGraphicsContext(IGraphicsContext * ctx)
{
    delete ctx;
}

}