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
#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/Context_private.h>
#include <PlastilinaCore/Plastilina.h>

namespace core
{
void Context::Impl::createGlCtx(const CtxAttributeList& attributes)
{
    std::vector<int> atrList;

    auto it = attributes.begin();
    for (; it != attributes.end(); ++it)
    {
        CtxAttribute name  = *it;
        CtxAttribute value = CTX_ATR_NULL;
        switch (name)
        {
        //                case CTX_ATR_GL_NSVIEW:
        //                    ++it;
        //                    value = *it;
        //                    if (!setAttribute(name, value) ) {
        //                        std::cerr << "failed to set attribute" <<
        //                        std::endl;
        //                    };
        //                    break;
        case CTX_ATR_GL_COLOR_SIZE:
            ++it;
            value = *it;
        //                    atrList.push_back(NSOpenGLPFAColorSize);
        //                    atrList.push_back((NSOpenGLPixelFormatAttribute)value);
        case CTX_ATR_GL_DEPTH_SIZE:
            ++it;
            value = *it;
        //                    atrList.push_back(NSOpenGLPFADepthSize);
        //                    atrList.push_back((NSOpenGLPixelFormatAttribute)value);
        case CTX_ATR_GL_STENCIL_SIZE:
            ++it;
            value = *it;
        //                    atrList.push_back(NSOpenGLPFAStencilSize);
        //                    atrList.push_back((NSOpenGLPixelFormatAttribute)value);
        case CTX_ATR_GL_ACCUM_SIZE:
            ++it;
            value = *it;
        //                    atrList.push_back(NSOpenGLPFAStencilSize);
        //                    atrList.push_back((NSOpenGLPixelFormatAttribute)value);
        default:
            break;
        }
    }
}

void Context::Impl::createClCtx(const CtxAttributeList& attributes) {}

bool Context::Impl::attribute(CtxAttribute name, CtxAttribute* value)
{
    switch (name)
    {
    //            case CTX_ATR_GL_NSVIEW:
    //                if (value) *value = (CtxAttribute)view;
    //                break;

    default:
        return false;
        break;
    }
    return true;
}

bool Context::Impl::setAttribute(CtxAttribute name, CtxAttribute value)
{
    switch (name)
    {
    //            case CTX_ATR_GL_NSVIEW:
    //                view = (NSView*)value;
    //                break;

    default:
        return false;
        break;
    }
    return true;
}
};
