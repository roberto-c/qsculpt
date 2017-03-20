/***************************************************************************
 *   Copyright (C) 2013 by Juan Roberto Cabral Flores                      *
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
#ifndef __PLASTILINACORE_GPUSUBDIVISIONRENDERABLE__
#define __PLASTILINACORE_GPUSUBDIVISIONRENDERABLE__

#include <PlastilinaCore/IRenderer.h>

class VertexBuffer;
struct SmoothVtxStruct;

namespace core
{
class GpuSubdivision;

class GpuSubdivisionRenderable : public IRenderable
{
  public:
    GpuSubdivisionRenderable(const core::GpuSubdivision* surface);

    virtual ~GpuSubdivisionRenderable();

    virtual void render(RenderState& state) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> _d;
};
};
#endif
