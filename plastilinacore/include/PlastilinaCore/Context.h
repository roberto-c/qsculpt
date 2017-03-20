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

namespace core
{
typedef intptr_t                  CtxAttribute;
typedef std::vector<CtxAttribute> CtxAttributeList;

enum
{
    CTX_ATR_NULL = 0,
    CTX_ATR_GL_NSVIEW,
    CTX_ATR_GL_QTWIDGET,
    CTX_ATR_GL_DOUBLE_BUFFER,
    CTX_ATR_GL_COLOR_SIZE,
    CTX_ATR_CL_ALPHA_SIZE,
    CTX_ATR_GL_DEPTH_SIZE,
    CTX_ATR_GL_STENCIL_SIZE,
    CTX_ATR_GL_ACCUM_SIZE,
    CTX_ATR_CL_SHARE_GL,
    CTX_ATR_VK_DOUBLE_BUFFER,
    CTX_ATR_BACKEND_GL,
    CTX_ATR_BACKEND_VK
};

class DLLEXPORT Context
{
    struct Impl;
    std::unique_ptr<Impl> d;

  public:
    Context(const CtxAttributeList& oclctx);

    Context(core::Context& oclctx);

    ~Context();

    CtxAttribute attribute(CtxAttribute attribute);

    void setAttribute(CtxAttribute attributeName, CtxAttribute value);
};
}
