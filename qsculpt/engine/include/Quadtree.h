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

//#include <QtDebug>
#include <vector>
//#include "Point3D.h"

struct CPointF
{
	float x, y;
	
	CPointF() : x(0.0f), y(0.0f) {};
	CPointF(float _x, float _y) : x(_x), y(_y){};
	
	bool operator==(const CPointF& p) const {
		return !((x > p.x + 0.0001) || (x <= p.x - 0.0001)
		(y > p.y + 0.0001) && (y <= p.y - 0.0001));
	}
	
	float length() { return x*x + y*y;};
	
	CPointF operator/(float c) { return CPointF(x/c, y/c);}
	CPointF operator*(float c) { return CPointF(x*c, y*c);}
	CPointF operator+(CPointF p) { return CPointF(x+p.x, y+p.y);}
};

template< typename T >
class Quadtree
	{
	public:
		Quadtree() {
			m_rootNode = new QuadtreeNode<T>(this);
			m_rootNode->setMinimumCoords(CPointF(-1024, -1024));
			m_rootNode->setMaximumCoords(CPointF(1024, 1024));
		}
		
		Quadtree(const Quadtree<T>& cpy) {
			m_rootNode = new QuadtreeNode<T>(this);
			m_rootNode->setMinimumCoords(CPointF(-1024, -1024));
			m_rootNode->setMaximumCoords(CPointF(1024, 1024));
			
			m_data = cpy.m_data;
		}
		
		~Quadtree() {
			delete m_rootNode;
		}
		
		inline T* getData() {
			return m_data.data();
		}
		
		inline T& operator[](int index) {
			return m_data[index];
		}
		
		inline const T& operator[](int index) const {
			return m_data[index];
		}
		
		inline const T& at(int index) const {
			return m_data.at(index);
		}
		
		inline bool contains(const T& v) const {
			return m_rootNode->contains(v);
		}
		
		inline void append(const T& v) {
			//qDebug("Quadtree::add");
			if (!m_rootNode->add(v))
			{
				//qDebug("Quadtree::add : failed: P: %s",
				//  qPrintable(v.vertex.toString()));
			}
		}
		
		inline void remove(const T& v) {
			//Q_UNUSED(v);
		}
		
		inline void remove(int index) {
			//Q_UNUSED(index);
		}
		
		inline void clear() {
			m_rootNode->clear();
			m_data.clear();
		}
		
		inline int size() const {
			return m_data.size();
		}
		
		inline int findClosest(const CPointF& v) const {
			T data;
			if (m_rootNode->findClosest(v, &data)) {
				return m_rootNode->indexOf(data);
			}
			return -1;
		}
		
		inline void reserve(int _size) {
			m_data.reserve(_size);
		}
		
		inline std::string toString() {
			return m_rootNode->toString();
		}
		
		inline std::vector<T> toVector() {
			return m_data;
		}
		
		inline const std::vector<T> toVector() const {
			return m_data;
		}
		
	private:
		template< typename D >
		class QuadtreeNode {
			static const int MaxVertex = 4;
			static const int MaxDepth = 50;
		public:
			QuadtreeNode(Quadtree<D>* octree);          
			QuadtreeNode(QuadtreeNode<D>* parent, int depth = 0);           
			~QuadtreeNode();
            
			inline void setMinimumCoords(const CPointF& v);          
			inline CPointF getMinimumCoords();       
			inline void setMaximumCoords(const CPointF& v);      
			inline CPointF getMaximumCoords();       
			bool findClosest(const CPointF& v, D* data) ;            
			int indexOf(const D& data);     
			inline bool contains(const D& data);        
			bool add(const D& data) ;       
			bool remove(const D&);
			void clear();
			
			//QString toString();
            
		protected:
			void doPartition();
			
			bool isInVolume(const D& v);
            
		private:
			QuadtreeNode(const QuadtreeNode<D>&);
			QuadtreeNode& operator=(const QuadtreeNode<D>&);
            
			Quadtree<D>*              m_octree;           // Pointer to owner octree
			QuadtreeNode<D>*          m_parent;           // Pointer to parent node
			std::vector<QuadtreeNode<D>*> m_childrenNodes;    // Children nodes.
			std::vector<int>            m_dataIndices;  // 
			CPointF                  m_minimumCoords;
			CPointF                  m_maximumCoords;
			bool                    m_hasChildren;
			int                     m_depth;
		};
		
		// Do not allow copying semantincs
		Quadtree& operator=(const Quadtree<T>&);
		
		QuadtreeNode<T>*	m_rootNode;
		std::vector<T>      m_data;
		
	};

template<typename T>
template<typename D>
Quadtree< T >::QuadtreeNode< D >::QuadtreeNode(Quadtree<D>* octree)
: m_octree(octree),
m_parent(NULL),
m_hasChildren(false),
m_depth (0) 
{                
    //m_minimumCoords = CPointF::null();
    //m_maximumCoords = CPointF::null();
}

template<typename T>
template<typename D>
Quadtree<T>::QuadtreeNode<D>::QuadtreeNode(QuadtreeNode<D>* parent, int depth)
: m_parent(parent),
m_hasChildren(false),
m_depth (depth) 
{
    m_octree = m_parent != NULL ? m_parent->m_octree : NULL;
    //m_minimumCoords = CPointF::null();
    //m_maximumCoords = CPointF::null();
}

template<typename T>
template<typename D>
Quadtree<T>::QuadtreeNode<D>::~OctreeNode() 
{
    clear();
}

template<typename T>
template<typename D>
inline void Quadtree<T>::QuadtreeNode<D>::setMinimumCoords(const CPointF& v)
{
    m_minimumCoords = v;
}

template<typename T>
template<typename D>
inline CPointF Quadtree<T>::QuadtreeNode<D>::getMinimumCoords() 
{
    return m_minimumCoords;
}

template<typename T>
template<typename D>
inline void Quadtree<T>::QuadtreeNode<D>::setMaximumCoords(const CPointF& v) 
{
    m_maximumCoords = v;
}

template<typename T>
template<typename D>
inline CPointF Quadtree<T>::QuadtreeNode<D>::getMaximumCoords() 
{
    return m_maximumCoords;
}

template<typename T>
template<typename D>
void Quadtree<T>::QuadtreeNode<D>::clear() 
{
    int size = m_childrenNodes.size();
    QuadtreeNode<D>* d = NULL;
    for(int i = 0; i < size; ++i) {
        d = m_childrenNodes[i];
        if (d) {
            d->clear();
            delete d;
        }
    }
    m_childrenNodes.clear();
    m_dataIndices.clear();
}

template<typename T>
template<typename D>
bool Quadtree<T>::QuadtreeNode<D>::findClosest(const CPointF& v, D* data) 
{
	typedef std::vector<Quadtree::QuadtreeNode<D> > QuadtreeNodeVector;
    if (data == NULL)
        return false;
    
    //if (!isInVolume(v))
    //  return CPointF::null();;
    
    if (m_hasChildren) {
        bool res;
		QuadtreeNodeVector::iterator itEnd = m_childrenNodes.end();
		for (QuadtreeNodeVector::iterator it = m_childrenNodes.begin(); it != itEnd; ++it)
		{
			res = it->findClosest(v, data);
            if (res) {
                return res;
            }
        }
    }
    else {
        if (m_dataIndices.isEmpty())
            return false;
        
        double distance = 0.0, minDistance = -1.0;
        int i = 0, indexOf = 0, dataSize = m_dataIndices.size();
        //qDebug("QuadtreeNode: dataSize = %d", dataSize);
        D tmp;
        do {
            tmp = m_octree->toVector().at(m_dataIndices[i]);
            distance = (v - tmp).length();
            if (distance <= minDistance || minDistance < 0.0) {
                minDistance = distance;
                indexOf = m_dataIndices[i];
            }
            ++i;
        } while(i < dataSize);
        *data = m_octree->toVector().at(indexOf);
        return true;
    }
    return false;
}

template<typename T>
template<typename D>
int Quadtree<T>::QuadtreeNode<D>::indexOf(const D& data) 
{
    int index = -1;
    
    //if (data == D::null()) {
    //  qDebug("QuadtreeNode::indexOf: CPointF cannot be null()");
    //  return false;
    //}
    
    if (!isInVolume(data))
        return false;
    
    if (m_hasChildren) {
        if (m_childrenNodes.isEmpty())
            return index;
        
        int size = m_childrenNodes.size();
        for (int i = 0; i < size; ++i) {
            index = m_childrenNodes[i]->indexOf(data);
            if (index)
                break;
        }
    }
    else {
        if (m_dataIndices.isEmpty())
            return index;
        
        int size = m_dataIndices.size();
        for (int i = 0; i < size; ++i) {
            if (m_octree->at(m_dataIndices[i]) == data) {
                index = m_dataIndices[i];
                break;
            }
        }
    }
    return index;           
}

template<typename T>
template<typename D>
inline bool Quadtree<T>::QuadtreeNode<D>::contains(const D& data) 
{
    return indexOf(data) != -1;
}

template<typename T>
template<typename D>
bool Quadtree<T>::QuadtreeNode<D>::add(const D& data) 
{
    // TODO: This method doesn't work, crash!!!
    if ( !isInVolume(data) )
        return false;
    
    if ( m_hasChildren ) {
		
        foreach(QuadtreeNode<D>* n, m_childrenNodes) {
            if ( n->add(data) )
                return true;
        }
    }
    else {
        if ( m_dataIndices.size() < MaxVertex || m_depth >= MaxDepth ) {
            m_octree->m_data.append(data);
            int index = m_octree->m_data.size() - 1;
            m_dataIndices.append(index);
            return true;
        }
        else {
            doPartition();
            foreach(QuadtreeNode<D>* n, m_childrenNodes) {
                if ( n->add(data) )
                    return true;
            }
        }
    }
    return false;
}

template<typename T>
template<typename D>
void Quadtree<T>::QuadtreeNode<D>::doPartition()
{
	CPointF delta = (m_maximumCoords - m_minimumCoords) / 2.0f;
    
    QuadtreeNode<D>* node = new QuadtreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(m_minimumCoords);
    node->setMaximumCoords(m_minimumCoords + delta);
    m_childrenNodes.append(node);
    
    CPointF tmp = m_minimumCoords;
    tmp.x = tmp.x + delta.x;
    node = new QuadtreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);
    
    tmp = m_minimumCoords;
    tmp.x = tmp.x + delta.x;
    tmp.y = tmp.y + delta.y;
    node = new QuadtreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);
    
    tmp = m_minimumCoords;
    tmp.y = tmp.y + delta.y;
    node = new QuadtreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);
    
	QuadtreeNode* n;
    int dataCount = m_dataIndices.size();
    for (int i = 0; i < dataCount; ++i)
    {
        foreach(n, m_childrenNodes)
        {
            if(n->isInVolume(m_octree->at(m_dataIndices[i])))
            {
                n->m_dataIndices.append(m_dataIndices[i]);
                break;
            }
        }
    }
    m_dataIndices.clear();
    m_hasChildren = true;
}

template<typename T>
template<typename D>
bool Quadtree<T>::QuadtreeNode<D>::isInVolume(const D& d)
{
    CPointF v = d;
    float x = v.x, y = v.y;
    if ( x < m_minimumCoords.x || x >= m_maximumCoords.x
		|| y < m_minimumCoords.y || y >= m_maximumCoords.y
		) 
    {
        return false;
    }
    
    return true;
}

template<typename T>
template<typename D>
std::string Quadtree<T>::QuadtreeNode<D>::toString()
{
    std::string res = "";
    return res;
}

#endif

