/***************************************************************************
 *   Copyright (C) 2007 by Juan Roberto Cabral Flores   *
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
#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <CoreEngine/Point3D.h>
#include <CoreEngine/ICollection.h>

namespace data
{
    template< typename T >
    class QuadTree : public ICollection<T>
    {
    public:
        QuadTree() ;

        QuadTree(const QuadTree<T>& cpy) ;

        ~QuadTree() ;

        /**
         * Add a new element to the collection.
         *
         * @param element element to add to the collection.
         */
        void add(T *element);

        /**
         * Removes a new element from the collection.
         *
         * @param element element to remove from the collection.
         */
        void remove(T *element);

        /**
         * Find the closest element to the point p.
         *
         * This function find the closest primitive to the point p that is whitin
         * a maxDistance radius. By default the radius is -1, meaning that we don't
         * care the minimum distance.
         *
         * @param p query point to which we want to find the closest point to that
         * @param maxDistance look for the closest points within this radius. If
         * maxDistance is negative, just look for the closest point, no matter at what
         * distance it is.
         */
        T* findClosest(const Point3& p, float maxDistance = 0);

        /**
         * Overload method @see findClosest
         */
        const T* findClosest(const Point3& p, float maxDistance = 0) const;

        /**
         * Find an element that a given ray intersects it.
         *
         * It returns the first element found that intersect the ray. This does
         * not mean that it is the closest element with respect the ray.
         *
         * @param ray Ray to use for intersection test against every element.
         * @param bag A ICollection object to insert the element found.
         */
        T* findFirstIntersect(const Ray& ray);

        /**
         * Find all the elements that intersects a given ray.
         *
         * This method returns all element which pass through a given ray. Each
         * element found is inserted in the collection bag. If no bag is passed
         * (bag = NULL), then it just return if an intersections has occured.
         *
         * @param ray Ray to use for intersection test against every element.
         * @param bag A ICollection object to insert the element found.
         *
         * @returns a boolean value stating if an intersection acurred.
         */
        bool findIntersect(const Ray& ray, ICollection<T*> bag = NULL);

        /**
         *
         */
        T* findIntersect(const AABB& box, ICollection<T*> bag = NULL);

        Iterator<T> iterator();

        std::vector<T *> toStdVector();

        std::set<T *> toStdSet();

    private:
        template< typename D >
        class QuadTreeNode;

        // Do not allow copying semantincs
        QuadTree& operator=(const QuadTree<T>&);

        QuadTreeNode<T>*    _rootNode;
        std::vector<T>      _data;
    };
}
#endif

