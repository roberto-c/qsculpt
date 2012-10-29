/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
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

#ifndef MESHCONTROLLER_H
#define MESHCONTROLLER_H

#include "IRenderable.h"
#include <PlastilinaCore/Point3D.h>

class ISurface;

/**
 * Used to manipulate a mesh.
 */
class SurfaceViewController : public IRenderable
{
public:
    /**
     * Constructor of a controller
     *
     * @param surface surface to which this controller sends commands to
     */
    SurfaceViewController(ISurface* surface);
    
    virtual ~SurfaceViewController();
    
    /**
     *
     */
    void setPosition(const Point3& pos);
    Point3 position() const;
    
    /**
     * Rotates the surface around a given axis by the given angle.
     */
    void setRotation(const Vector3& axis, float angle);
    void setRotation(const Eigen::Quaternionf& r);
    Eigen::Quaternionf rotation();

    
    void paintGL();
    
private:
    struct PrivateData;
    PrivateData* d;
};

#endif // MESHCONTROLLER_H
