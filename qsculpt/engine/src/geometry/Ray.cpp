/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
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
#include "geometry/Ray.h"
#include "geometry/Plane.h"

namespace geometry
{

    float      Ray::DEFAULT_EPSILON= 0.0001;
    Point3     Ray::DEFAULT_ORIGIN = Point3(0, 0, 0);
    Vector3    Ray::DEFAULT_DIR    = Vector3(1, 0, 0);

    Ray::Ray(const Point3 &o, const Vector3 &d)
        : Eigen::ParametrizedLine<float, 3>(o, d)
    {
    }

    float Ray::intersect(const Ray &ray, Point3 *p, float /*ep*/) const
    {
        return -1.0f;
    }

    float Ray::intersect(const Plane &plane, Point3 *p, float /*ep*/) const
    {
        // TODO: ugly. modify Eigen::ParamtrizedLine class
        float t = const_cast<Ray*>(this)->intersection(plane);
        if (p && t >= 0) {
            *p = origin() + direction() * t;
        }
        return t;
    }

    float Ray::intersect(const Point3 &point, Point3 *p, float ep) const
    {
        float d = distance(point);
        if (d < ep) {
            if (p) *p = point;
            double l = (point - this->m_origin).norm();
            return l / this->m_direction.norm();
        }
        return -1.0f;
    }
}
