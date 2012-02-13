/***************************************************************************
*   Copyright (C) 2011 by Juan Roberto Cabral Flores   *
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
#ifndef OCTREE_H
#define OCTREE_H

#include <QtCore/QtDebug>
#include <QtCore/QVector>
#include <vector>
#include <set>

#include "Point3D.h"
#include "ICollection.h"
#include "OctreeNode.h"
#include "data/Functors.h"
#include "geometry/Aabb.h"
#include "geometry/Ray.h"

namespace data {
    template<typename T, typename ToPointFn=data::ImplicitConvToPoint<T> > class Octree;
    
    template< typename Type, typename ToPointFn>
    class Octree : public data::ICollection<Type>
    {
        typedef OctreeNode<Type,ToPointFn> NodeType;
        
    public:
        Octree() {
            m_rootNode = new OctreeNode<Type,ToPointFn>(this);
            m_rootNode->setMinimumCoords(Point3(-1024, -1024, -1024));
            m_rootNode->setMaximumCoords(Point3(1024, 1024, 1024));
        }
        
        Octree(const Octree<Type,ToPointFn>& cpy) {
            m_rootNode = new NodeType(this);
            m_rootNode->setMinimumCoords(Point3(-1024, -1024, -1024));
            m_rootNode->setMaximumCoords(Point3(1024, 1024, 1024));
            
            foreach (Point3 v, m_data) {
                append(v);
            }
        }
        
        ~Octree() {
            delete m_rootNode;
        }
        
        inline void add(const Type & v) {
            if (!m_rootNode->add(v))
            {
                throw std::runtime_error("Failed to add element");
            }
        }
        
        inline int size() {
            return m_data.size();
        }
        
        inline int findClosest(const Point3& v) const {
            Type data;
            if (m_rootNode->findClosest(v, &data)) {
                return m_rootNode->indexOf(data);
            }
            return -1;
        }
        
        inline bool findClosest(Type&, const Point3&,float) const { 
            throw std::runtime_error("Not implemented"); 
        }
        
        inline bool findFirstIntersect(Type&, const geometry::Ray&) const { 
            throw std::runtime_error("Not implemented"); 
        }
        
        inline bool findIntersect(const geometry::Ray&, 
                                  data::ICollection<Type>*) const {
            throw std::runtime_error("Not implemented"); 
        }
        
        inline bool findIntersect(const geometry::AABB&, 
                                  data::ICollection<Type>*) const {
            throw std::runtime_error("Not implemented"); 
        }
        
        inline Type* getData() {
            return m_data.data();
        }
        
        inline Type& operator[](int index) {
            return m_data[index];
        }
        
        inline const Type& operator[](int index) const {
            return m_data[index];
        }
        
        inline const Type& at(int index) const {
            return m_data.at(index);
        }
        
        inline bool contains(const Type& v) const {
            return m_rootNode->contains(v);
        }
        
        inline bool append(const Type& v) {
            //qDebug("Octree::add");
            if (!m_rootNode->add(v))
            {
                qDebug("%s", qPrintable("Octree::add : failed: Point: " + ::toString(v)));
                return false;
            }
            return true;
        }
        
        inline void remove(const Type& v) {
            Q_UNUSED(v);
        }
        
        inline void remove(int index) {
            Q_UNUSED(index);
        }
        
        inline void clear() {
            m_rootNode->clear();
            m_data.clear();
        }
        
        inline int size() const {
            return m_data.size();
        }
        
        inline void reserve(int _size) {
            m_data.reserve(_size);
        }
        
        inline QString toString() {
            return m_rootNode->toString();
        }
        
        inline QVector<Type> toQVector() {
            return m_data;
        }
        
        inline const QVector<Type> toQVector() const {
            return m_data;
        }
        
        /**
         * Creates an iterator over the collection class
         */
        inline Iterator<Type> iterator() {
            return Iterator<Type>(NULL);
        }
        
        std::vector<Type> toStdVector() {
            return std::vector<Type>();
        }
        
        std::set<Type> toStdSet() {
            return std::set<Type>(); 
        }
        
    private:
        // Do not allow coping semantincs
        Octree& operator=(const Octree<Type,ToPointFn>&);
        
        NodeType*  m_rootNode;
        QVector<Type>      m_data;
      
        friend class OctreeNode<Type,ToPointFn>;
    };
    
    
}

#endif

