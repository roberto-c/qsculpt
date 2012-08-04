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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/geometry/Plane.h>
#include <PlastilinaCore/geometry/Sphere.h>

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
    
    float Ray::intersect(const Sphere& sphere, Point3 *p, float /*ep*/) const
    {
        //Compute A, B and C coefficients
        float a = direction().dot(direction());
        float b = 2 * direction().dot(origin());
        float c = origin().dot(origin()) - (sphere.radius() * sphere.radius());
        float t = -1.f;
        
        //Find discriminant
        float disc = b * b - 4 * a * c;
        
        // if discriminant is negative there are no real roots, so return 
        // false as ray misses sphere
        if (disc < 0)
            return t;
        
        // compute q as described above
        float distSqrt = sqrtf(disc);
        float q;
        if (b < 0)
            q = (-b - distSqrt)/2.0;
        else
            q = (-b + distSqrt)/2.0;
        
        // compute t0 and t1
        float t0 = q / a;
        float t1 = c / q;
        
        // make sure t0 is smaller than t1
        if (t0 > t1)
        {
            // if t0 is bigger than t1 swap them around
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }
        
        // if t1 is less than zero, the object is in the ray's negative direction
        // and consequently the ray misses the sphere
        if (t1 < 0)
            return t;
        
        // if t0 is less than zero, the intersection point is at t1
        if (t0 < 0)
        {
            t = t1;
        }
        // else the intersection point is at t0
        else
        {
            t = t0;
        }
        // calculate point of intersection
        if (p && t >= 0) {
            *p = origin() + direction() * t;
        }
        return t;
    }
}
