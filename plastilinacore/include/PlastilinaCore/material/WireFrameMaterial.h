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

#include <iostream>

#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/Material.h>

class WireFrameMaterial : public Material
{
  public:
    WireFrameMaterial();

    virtual ~WireFrameMaterial();

    virtual void load();

    virtual void unload();

    virtual void setup(const std::shared_ptr<SceneNode>& doc);

    virtual void setup(const std::shared_ptr<const SceneNode>& doc);

    /**
     * Returns the color used for outlines
     */
    Color outlineColor() const;

    /**
     * Set the color used for outlines
     */
    void setOutlineColor(const Color& c);

    /**
     * Returns the color used for fill primitives
     */
    Color fillColor() const;

    /**
     * Set the color used for fill a primitive
     */
    void setFillColor(const Color& c);

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
