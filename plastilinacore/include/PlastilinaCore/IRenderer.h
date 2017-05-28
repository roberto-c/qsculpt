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
#ifndef IRENDERER_H_
#define IRENDERER_H_

#include <PlastilinaCore/PlastilinaDefs.h>

class ISurface;
class Material;
class SceneNode;
struct RenderState;

enum class RenderMode
{
    RM_WireFrame,
    RM_Flat,
    RM_Smooth,
    RM_Textured,
    RM_Points
};

class DLLEXPORT IRenderer
{
  public:
    virtual ~IRenderer() {}

    /**
     * This method must be implemented by a derived class. This method
     * dispatches the commands need it to render objects on the display.
     */
    virtual void renderObject(std::shared_ptr<SceneNode>& node) = 0;
};

class DLLEXPORT IRenderable
{
  public:
    virtual ~IRenderable(){};

    virtual void render(RenderState& state) const = 0;
};

#endif /*IRENDERER_H_*/
