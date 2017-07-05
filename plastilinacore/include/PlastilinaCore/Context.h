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

#include <memory>
#include <vector>

namespace core
{
typedef intptr_t                  CtxAttribute;
typedef std::vector<CtxAttribute> CtxAttributeList;
typedef intptr_t                          PixelFormatAttribute;
typedef std::vector<PixelFormatAttribute> PixelFormatAttributeList;

class IDevice;

enum class ContextType : uint32_t
{
    OpenGL,
    Vulkan,
    OpenCL
};

struct GraphicsContextCreateInfo
{
    ContextType       contextType;     //!< Type of context to create
    intptr_t          osWindowHandle;  //!< Native window handle
    intptr_t          osHandleEx;      //!< Extra OS handle to use (platform dependent)
    intptr_t          osContext;       //!< Native context (GL context, etc)
                                       //!< If set then context created is just a wrapper
                                       //!< around this handle (all other attributes
                                       //!< are ignored)
    CtxAttributeList  attributesList;  //!< Attributes that the context should have
    PixelFormatAttributeList   pixelFormatAttributes; //!< Attributes for the surface format

    GraphicsContextCreateInfo()
        : contextType(ContextType::OpenGL)
        , osWindowHandle(0)
        , osHandleEx(0)
        , osContext(0)
    {}
};

class DLLEXPORT IGraphicsContext
{
public:
    static IGraphicsContext * createGraphicsContext(GraphicsContextCreateInfo & createInfo);
    static void               destroyGraphicsContext(IGraphicsContext * ctx);

    virtual ContextType contextType() const = 0;

    virtual bool makeCurrent() = 0;
    virtual bool swapBuffers() = 0;

    virtual IDevice* device() = 0;
    virtual void resize(int width, int height) = 0;

protected:
    IGraphicsContext() {}
    virtual ~IGraphicsContext(){}
};

}
