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
#include "StdAfx.h"
#include "Plane.h"
#include "ray.h"

namespace geometry
{
    float Plane::DEFAULT_TOL = 0.0001f;
    Point3 Plane::DEFAULT_POSITION = Point3(0, 0, 0);
    Vector3 Plane::DEFAULT_ORIENTATION = Vector3(0, 0, 1);

    Plane::Plane(const Point3 & c, const Vector3& n)
        : Eigen::Hyperplane<float, 3>(n, c)
    {
    }

    float Plane::intersect(const Ray &ray, Point3 *p, float ep) const
    {
        return ray.intersect(*this, p, ep);
    }
}
