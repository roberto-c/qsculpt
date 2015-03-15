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

#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Quadtree.h>

namespace data
{
    template< typename T >
    template< typename D >
    class QuadTree<T>::QuadTreeNode {
        static const int MaxVertex = 4;
        static const int MaxDepth = 50;
    public:
        QuadTreeNode(QuadTree<D>* octree);
        QuadTreeNode(QuadTreeNode<D>* parent, int depth = 0);
        ~QuadTreeNode();

        inline void setMinimumCoords(const Point3& v);
        inline Point3 getMinimumCoords();
        inline void setMaximumCoords(const Point3& v);
        inline Point3 getMaximumCoords();
        bool findClosest(const Point3& v, D* data) ;
        int indexOf(const D& data);
        inline bool contains(const D& data);
        bool add(const D& data) ;
        bool remove(const D&);
        void clear();

        //std::string toString();

    protected:
        void doPartition();

        bool isInVolume(const D& v);

    private:
        QuadTreeNode(const QuadTreeNode<D>&);
        QuadTreeNode& operator=(const QuadTreeNode<D>&);

        QuadTree<D>*              _quadtree;           // Pointer to owner octree
        QuadTreeNode<D>*          _parent;           // Pointer to parent node
        std::vector<QuadTreeNode<D>*> _children;    // Children nodes.
        std::vector<int>            _dataIndices;  //
        Point3                  _minimumCoords;
        Point3                  _maximumCoords;
        bool                    _hasChildren;
        int                     _depth;
    };


    template< typename T > template< typename D >
    QuadTree<T>::QuadTreeNode<D>::QuadTreeNode(QuadTree<D>* quadtree)
        : _quadtree(quadtree),
        _parent(NULL),
        _hasChildren(false),
        _depth(0)
    {
    }

    template< typename T > template< typename D >
    QuadTree<T>::QuadTreeNode<D>::QuadTreeNode(QuadTreeNode<D>* parent, int depth)
        : _quadtree(NULL),
        _parent(parent),
        _hasChildren(false),
        _depth(0)
    {
        if (parent) _quadtree = parent->_quadtree;
    }

    template< typename T > template< typename D >
    QuadTree<T>::QuadTreeNode<D>::~QuadTreeNode()
    {
    }

    template< typename T > template< typename D >
    void QuadTree<T>::QuadTreeNode<D>::setMinimumCoords(const Point3& v)
    {

    }

    template< typename T > template< typename D >
    Point3 QuadTree<T>::QuadTreeNode<D>::getMinimumCoords()
    {
        return Point3();
    }

    template< typename T > template< typename D >
    void QuadTree<T>::QuadTreeNode<D>::setMaximumCoords(const Point3& v)
    {

    }

    template< typename T > template< typename D >
    Point3 QuadTree<T>::QuadTreeNode<D>::getMaximumCoords()
    {
        return Point3();
    }

    template< typename T > template< typename D >
    bool QuadTree<T>::QuadTreeNode<D>::findClosest(const Point3& v, D* data)
    {
        return false;
    }

    template< typename T > template< typename D >
    int QuadTree<T>::QuadTreeNode<D>::indexOf(const D& data)
    {
        return -1;
    }
    template< typename T > template< typename D >
    bool QuadTree<T>::QuadTreeNode<D>::contains(const D& data)
    {
        return false;
    }

    template< typename T > template< typename D >
    bool QuadTree<T>::QuadTreeNode<D>::add(const D& data)
    {
        return false;
    }

    template< typename T > template< typename D >
    bool QuadTree<T>::QuadTreeNode<D>::remove(const D&)
    {
        return false;
    }

    template< typename T > template< typename D >
    void QuadTree<T>::QuadTreeNode<D>::clear()
    {

    }





    template<typename T>
    QuadTree<T>::QuadTree() {
        _rootNode = new QuadTreeNode<T>(this);
        _rootNode->setMinimumCoords(Point3(-1024, -1024, 0));
        _rootNode->setMaximumCoords(Point3(1024, 1024, 0));
    }

    template<typename T>
    QuadTree<T>::QuadTree(const QuadTree<T>& cpy) {
        _rootNode = new QuadTree<T>(this);
        _rootNode->setMinimumCoords(Point3(-1024, -1024, 0));
        _rootNode->setMaximumCoords(Point3(1024, 1024, 0));

        _data = cpy.m_data;
    }

    template<typename T>
    QuadTree<T>::~QuadTree() {
        delete _rootNode;
    }

    /**
     * Add a new element to the collection.
     *
     * @param element element to add to the collection.
     */
    template<typename T>
    void QuadTree<T>::add(T *element)
    {

    }

    /**
     * Removes a new element from the collection.
     *
     * @param element element to remove from the collection.
     */
    template<typename T>
    void QuadTree<T>::remove(T *element)
    {

    }

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
    template<typename T>
    T* QuadTree<T>::findClosest(const Point3& p, float maxDistance)
    {
        return NULL;
    }

    /**
     * Overload method @see findClosest
     */
    template<typename T>
    const T* QuadTree<T>::findClosest(const Point3& p, float maxDistance) const
    {
        return NULL;
    }

    /**
     * Find an element that a given ray intersects it.
     *
     * It returns the first element found that intersect the ray. This does
     * not mean that it is the closest element with respect the ray.
     *
     * @param ray Ray to use for intersection test against every element.
     * @param bag A ICollection object to insert the element found.
     */
    template<typename T>
    T* QuadTree<T>::findFirstIntersect(const Ray& ray)
    {
        return NULL;
    }

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
    template<typename T>
    bool QuadTree<T>::findIntersect(const Ray& ray, ICollection<T*> bag)
    {
        return false;
    }

    template<typename T>
    Iterator<T> QuadTree<T>::iterator()
    {
        return Iterator<T>(NULL);
    }

    template<typename T>
    std::vector<T *> QuadTree<T>::toStdVector()
    {
        return std::vector<T*>();
    }

    template<typename T>
    std::set<T *> QuadTree<T>::toStdSet()
    {
        return std::set<T*>();
    }
}
