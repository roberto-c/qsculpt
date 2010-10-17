/*
 *  Aabb.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 6/29/10.
 *  Copyright 2010 Juan Roberto Cabral. All rights reserved.
 *
 */

#ifndef AABB_H_
#define AABB_H_

#include <Eigen/Geometry>

namespace geometry
{
/**
 * @class AABB
 *
 * Axis aligned bounding box (AABB) class. Hold methods to make intersection 
 * test between other AABB, ray, line segment, sphere, etc.
 * 
 */
class AABB : public Eigen::AlignedBox<float, 3>
{
public:
    /**
     * Default constructor for AABB. The points a set to (0, 0, 0)
     */
    AABB() : Eigen::AlignedBox<float, 3>(){}
    
    /**
     * Constructor of a AABB that initializes the points to the values 
     * specified.
     *
     * @param p1 first point of the bounding box.
     * @param p2 second point of the bounding box.
     */
    AABB(const Point3& p1, const Point3& p2) : Eigen::AlignedBox<float, 3>(p1, p2){}
    
    /**
     *
     */
    inline bool intersect(const Eigen::AlignedBox<float, 3>& box);
};

}



inline bool geometry::AABB::intersect(const Eigen::AlignedBox<float, 3>& box)
{
    return ((box.min().x() < max().x() && box.max().x() > min().x()) &&
            (box.min().y() < max().y() && box.max().y() > min().y()) &&
            (box.min().z() < max().z() && box.max().z() > min().z()));
}

#endif
