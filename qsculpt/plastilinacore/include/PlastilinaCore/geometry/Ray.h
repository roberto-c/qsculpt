/***************************************************************************
 *   Copyright (C) 2011 by Juan Roberto Cabral Flores                      *
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

#ifndef RAY_H
#define RAY_H

#include <Eigen/Geometry>
#include <PlastilinaCore/Point3D.h>



namespace geometry
{
    class AABB;
    class Plane;
    class Sphere;

    class Ray : public Eigen::ParametrizedLine<float, 3>
    {
        static float      DEFAULT_EPSILON;
        static Point3     DEFAULT_ORIGIN;
        static Vector3    DEFAULT_DIR;

    public:
        /**
         * Empty constructor. Origin is (0, 0, 0) and direction is (1, 0, 0)
         */
        Ray(const Point3& o = DEFAULT_ORIGIN, const Vector3& d = DEFAULT_DIR);

        /**
         * Check if the ray intersects another ray and stores the intersection
         * point in p.
         *
         * @param ray ray to test intersection with.
         * @param p an output paramter to store the point where the intersection
         * happened. If null, not output is given.
         * @param ep is the tolerance value to use to say if there is an
         * intersection or not.
         *
         * @return a float value stating the time t in which the intersection
         * ocurs
         */
        float intersect(const Ray& ray, Point3 *p = NULL, 
                        float ep = DEFAULT_EPSILON) const;

        /**
         *
         */
        float intersect(const Plane& plane, Point3 *p = NULL, 
                        float ep = DEFAULT_EPSILON) const;
        
        /**
         * Check if a point lies in a ray.
         *
         * @param point point to test for.
         * @param p point where the intersection is. For this overload, it is 
         * the same as point
         */
        float intersect(const Point3& point, Point3 *p = NULL, 
                        float ep = DEFAULT_EPSILON) const;
        
        /**
         * Check if a ray intersects an sphere and stores the intersection point
         * in p.
         *
         * @param sphere
         */
        float intersect(const Sphere& sphere, Point3 *p = NULL,
                        float ep = DEFAULT_EPSILON) const;
    };
};

#endif // RAY_H
