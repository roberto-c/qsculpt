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

#ifndef PLANE_H
#define PLANE_H

#include <Eigen/Geometry>
#include "Point3D.h"

namespace geometry
{
    class Ray;
    /**
     * Class that represent a 3d plane.
     */
    class Plane : public Eigen::Hyperplane<float, 3>
    {
        static float DEFAULT_TOL;
        static Point3 DEFAULT_POSITION;
        static Vector3 DEFAULT_ORIENTATION;

    public:
        Plane(const Point3 &center = DEFAULT_POSITION, const Vector3 &normal = DEFAULT_ORIENTATION);

        float intersect(const Ray& ray, Point3 *p = NULL, float ep = DEFAULT_TOL) const;
    };
}
#endif // PLANE_H
