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
#ifndef OCTREE_H
#define OCTREE_H

#include <QVector>
#include "point3d.h"

template< typename T >
class Octree
{
public:
	Octree() {
		m_rootNode = new OctreeNode<T>(this);
		m_rootNode->setMinimumCoords(Vertex(-1000, -1000, -1000));
		m_rootNode->setMaximumCoords(Vertex(1000, 1000, 1000));
	}
	
	~Octree() {
		delete m_rootNode;
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
		qDebug("Octree::add");
		if (!m_rootNode->add(v))
			qDebug("Octree::add : failed: P: %s", qPrintable(v.vertex.toString()));
	}
	
	inline void remove(const T& v) {
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
	
	inline int findClosest(const Vertex& v) const {
		T data;
		if (m_rootNode->findClosest(v, &data)) {
			return m_rootNode->indexOf(data);
		}
		return -1;
	}
	
	inline QString toString() {
		return m_rootNode->toString();
	}
	
	inline QVector<T> toQVector() {
		return m_data;
	}
	
	inline const QVector<T> toQVector() const {
		return m_data;
	}
	
private:
	template< typename D >
	class OctreeNode {
		static const int MaxVertex = 4;
	public:
		OctreeNode(Octree<D>* octree);			
		OctreeNode(OctreeNode<D>* parent, int depth = 0);			
		~OctreeNode();
			
		inline void setMinimumCoords(const Vertex& v);			
		inline Vertex getMinimumCoords();		
		inline void setMaximumCoords(const Vertex& v);		
		inline Vertex getMaximumCoords();		
		bool findClosest(const Vertex& v, D* data) ;			
		int indexOf(const D& data);		
		inline bool contains(const D& data);		
		bool add(const D& data) ;		
		bool remove(const D&);
		void clear();
		
		QString toString();
			
	protected:
		void doPartition();
		
		bool isInVolume(const D& v);
			
	private:
		OctreeNode(const OctreeNode<D>&);
		OctreeNode& operator=(const OctreeNode<D>&);
			
		Octree<D>*				m_octree;			// Pointer to owner octree
		OctreeNode<D>*			m_parent;			// Pointer to parent node
		QVector<OctreeNode<D>*>	m_childrenNodes;	// Children nodes.
		QVector<int>			m_dataIndices;	// 
		Vertex					m_minimumCoords;
		Vertex					m_maximumCoords;
		bool					m_hasChildren;
		int						m_depth;
	};
	
	// Do not allow coping semantincs
	Octree(const Octree<T>&);
	Octree& operator=(const Octree<T>&);
	
	OctreeNode<T>*	m_rootNode;
	QVector<T>		m_data;
	
};

template<typename T>
template<typename D>
Octree< T >::OctreeNode< D >::OctreeNode(Octree<D>* octree)
: m_octree(octree),
m_parent(NULL),
m_hasChildren(false),
m_depth (0) {
				
	m_minimumCoords = Vertex::null();
	m_maximumCoords = Vertex::null();
}

template<typename T>
template<typename D>
Octree<T>::OctreeNode<D>::OctreeNode(OctreeNode<D>* parent, int depth)
: m_parent(parent),
m_hasChildren(false),
m_depth (depth) {
	m_octree = m_parent != NULL ? m_parent->m_octree : NULL;
	m_minimumCoords = Vertex::null();
	m_maximumCoords = Vertex::null();
}

template<typename T>
template<typename D>
Octree<T>::OctreeNode<D>::~OctreeNode() {
	clear();
}

template<typename T>
template<typename D>
inline void Octree<T>::OctreeNode<D>::setMinimumCoords(const Vertex& v){
	m_minimumCoords = v;
}

template<typename T>
template<typename D>
inline Vertex Octree<T>::OctreeNode<D>::getMinimumCoords() {
	return m_minimumCoords;
}

template<typename T>
template<typename D>
inline void Octree<T>::OctreeNode<D>::setMaximumCoords(const Vertex& v) {
	m_maximumCoords = v;
}

template<typename T>
template<typename D>
inline Vertex Octree<T>::OctreeNode<D>::getMaximumCoords() {
	return m_maximumCoords;
}

template<typename T>
template<typename D>
void Octree<T>::OctreeNode<D>::clear() {
	int size = m_childrenNodes.size();
	OctreeNode<D>* d = NULL;
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
bool Octree<T>::OctreeNode<D>::findClosest(const Vertex& v, D* data) {
	if (data == NULL)
		return false;
	
	//if (!isInVolume(v))
	//	return Vertex::null();;
	
	if (m_hasChildren) {
		bool res;
		foreach(OctreeNode<D>* node, m_childrenNodes) {
			res = node->findClosest(v, data);
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
		qDebug("OctreeNode: dataSize = %d", dataSize);
		D tmp;
		do {
			tmp = m_octree->toQVector().at(m_dataIndices[i]);
			distance = (v - tmp).length();
			if (distance <= minDistance || minDistance < 0.0) {
				minDistance = distance;
				indexOf = m_dataIndices[i];
			}
			++i;
		} while(i < dataSize);
		*data = m_octree->toQVector().at(indexOf);
		return true;
	}
	return false;
}

template<typename T>
template<typename D>
int Octree<T>::OctreeNode<D>::indexOf(const D& data) {
	int index = -1;
	
	//if (data == D::null()) {
	//	qDebug("OctreeNode::indexOf: Vertex cannot be null()");
	//	return false;
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
inline bool Octree<T>::OctreeNode<D>::contains(const D& data) {
	return indexOf(data) != -1;
}

template<typename T>
template<typename D>
bool Octree<T>::OctreeNode<D>::add(const D& data) {
	// TODO: This method doesn't work, crash!!!
	if ( !isInVolume(data) )
		return false;
	
	if ( m_hasChildren ) {
		foreach(OctreeNode<D>* n, m_childrenNodes) {
			if ( n->add(data) )
				return true;
		}
	}
	else {
		if ( m_dataIndices.size() < MaxVertex || m_depth >= 50 ) {
			m_octree->toQVector().append(data);
			int index = m_octree->toQVector().size() - 1;
			m_dataIndices.append(index);
			//qDebug("minCoords: %s, maxCoords: %s", 
			//	   qPrintable( m_minimumCoords.toString() ),
			//	   qPrintable( m_maximumCoords.toString() ) );
			return true;
		}
		else {
			doPartition();
			//qDebug("OctreeNode: childrenNodes count: %d", m_childrenNodes.size());
			foreach(OctreeNode<D>* n, m_childrenNodes) {
				if ( n->add(data) )
					return true;
			}
		}
	}
	return false;
}

template<typename T>
template<typename D>
void Octree<T>::OctreeNode<D>::doPartition()
{
	qDebug("Partition done at level: %d", m_depth + 1);
	Vertex delta = (m_maximumCoords - m_minimumCoords) / 2.0f;
	
	OctreeNode<D>* node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	Vertex tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	tmp.setY(tmp.getY() + delta.getY());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setY(tmp.getY() + delta.getY());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	tmp.setY(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setY(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode<D>(this, m_depth + 1);
	node->setMinimumCoords(tmp);
	node->setMaximumCoords(tmp + delta);
	m_childrenNodes.append(node);
	
	int dataCount = m_dataIndices.size();
	for (int i = 0; i < dataCount; ++i)
	{
		OctreeNode* n;
		foreach(n, m_childrenNodes)
		{
			if(n->isInVolume(m_octree->at(m_dataIndices[i])))
			{
				n->m_dataIndices.append(m_dataIndices[i]);
				//qDebug("Added...");
				//qDebug("new minCoords: %s, maxCoords: %s", 
				//	   qPrintable( n->m_minimumCoords.toString() ),
				//	   qPrintable( n->m_maximumCoords.toString() ) );
				break;
			}
		}
	}
	m_dataIndices.clear();
	m_hasChildren = true;
}

template<typename T>
template<typename D>
bool Octree<T>::OctreeNode<D>::isInVolume(const D& d)
{
	Vertex v = d;
	float x = v.getX(), y = v.getY(), z = v.getZ();
	if ( x < m_minimumCoords.getX() || x >= m_maximumCoords.getX()
		 || y < m_minimumCoords.getY() || y >= m_maximumCoords.getY()
		 || z < m_minimumCoords.getZ() || z >= m_maximumCoords.getZ()
		 ) {
		return false;
	}
	
	return true;
}

template<typename T>
template<typename D>
QString Octree<T>::OctreeNode<D>::toString()
{
	QString res;
	QString indent;
	for (int i = 0; i < m_depth; i++)
		indent+= "  ";
	
	res = indent + "Node{\n";
	res += indent + "m_depth:" + QString::number(m_depth) + "\n"; 
	res += indent + "m_minCoords:" + m_minimumCoords.toString() + "\n"; 
	res += indent + "m_maxCoords:" + m_maximumCoords.toString();
	qDebug("%s", qPrintable(res));
	if (m_hasChildren) {
		OctreeNode* n;
		res = "";
		foreach(n, m_childrenNodes)
		{
			res += n->toString();
		}
		//qDebug("%s", qPrintable(res));
	}
	else {
		res = "";
		int i;
		foreach(i, m_dataIndices)
		{
			res = indent + "point[" + QString::number(i)
				+ "]:" + m_octree->toQVector()[i].vertex.toString();
			qDebug("%s", qPrintable(res));
		}
	}
	qDebug(qPrintable(indent + "}"));
	//res = res + indent + "}\n";
	res = "";
	return res;
}

#endif
