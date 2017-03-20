/***************************************************************************
 *   Copyright (C) 2015 by Juan Roberto Cabral Flores                      *
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
#ifndef __PLASTILINACORE_PRIMITIVEFACTORY_H__
#define __PLASTILINACORE_PRIMITIVEFACTORY_H__

#include <PlastilinaCore/subdivision/GpuSubdivision.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <memory>

namespace core
{
class GpuSubdivision;
// class Subdivision;
class Mesh;

template <typename TSurface>
class PrimitiveFactory
{
  public:
    /**
     * Create a unit size box (all sides of size 1) with the center at
     * (0,0,0)
     *
     * @return TSurface a constructed ISurface box
     */
    static TSurface* createBox();

    static TSurface* createQuad(float w = 1.0f, float h = 1.0f);
};

template <>
class PrimitiveFactory<GpuSubdivision>;
template <>
class PrimitiveFactory<Subdivision>;
template <>
class PrimitiveFactory<Mesh>;
};

namespace core
{
template <>
class PrimitiveFactory<GpuSubdivision>
{
  public:
    static GpuSubdivision* createBox();
    static GpuSubdivision* createQuad(float w = 1.0f, float h = 1.0f);
};

template <>
class PrimitiveFactory<Subdivision>
{
  public:
    static Subdivision* createBox();
    static Subdivision* createQuad(float w = 1.0f, float h = 1.0f);
};
};

#endif
