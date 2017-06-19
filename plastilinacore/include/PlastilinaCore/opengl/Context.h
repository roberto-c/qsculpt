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

#include <vector>

#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/opengl/GlDevice.h>


#define PLASTILINA_GL_CONTEXT_HDC    0x80000000   // Pass HDC context to use
#define PLASTILINA_GL_CONTEXT_WHND   0x80000001   // Pass window handle to use

namespace gl
{
class Context : public core::IGraphicsContext
{
public:
    Context(const core::GraphicsContextCreateInfo & createInfo);

    virtual ~Context() override;

    virtual core::ContextType contextType() const override
    {
        return core::ContextType::OpenGL;
    }

    virtual bool makeCurrent() override;
    virtual bool swapBuffers() override;

    virtual core::IDevice* device() override;
    virtual void resize(int w, int h) override;

    std::vector<gl::Device> deviceList() const;

    intptr_t windowHandle() const;

    intptr_t nativeDeviceHandle() const;

    intptr_t nativeGlContext() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
};
