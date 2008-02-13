/*
 *  octree.cpp
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 1/17/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

//#include "octree.h"

/*
Octree::OctreeNode::OctreeNode(Octree* octree)
: m_octree(octree),
	m_parent(NULL),
	m_hasChildren(false)
{
	m_minimumCoords = Vertex::null();
	m_maximumCoords = Vertex::null();
}

Octree::OctreeNode::OctreeNode(OctreeNode* parent)
: m_parent(parent),
	m_hasChildren(false)
{
	m_octree = m_parent != NULL ? m_parent->m_octree : NULL;
	m_minimumCoords = Vertex::null();
	m_maximumCoords = Vertex::null();
}

Octree::OctreeNode::~OctreeNode()
{
}

void Octree::OctreeNode::setMinimumCoords(const Vertex& v)
{
	m_minimumCoords = v;
}
	
Vertex Octree::OctreeNode::getMinimumCoords()
{
	return m_minimumCoords;
}
	
void Octree::OctreeNode::setMaximumCoords(const Vertex& v)
{
	m_maximumCoords = v;
}
	
Vertex Octree::OctreeNode::getMaximumCoords()
{
	return m_maximumCoords;
}
	
Vertex Octree::OctreeNode::findClosest(const Vertex& v)
{
	if (!isInVolume(v))
		return Vertex::null();;
	
	if (m_hasChildren)
	{
		Vertex res = Vertex::null();
		foreach(OctreeNode* node, m_childrenNodes)
		{
			res = node->findClosest(v);
			if (res != Vertex::null())
				return res;
		}
	}
	else
	{
		if (m_vertexIndices.isEmpty())
			return Vertex::null();
		
		double distance = 0.0, minDistance = 0.0;
		int i = 0, indexOf = 0;
		Vertex vertex;
		do
		{
			vertex = m_octree->getVertexData().at(m_vertexIndices[i]);
			distance = (v - vertex).length();
			if (distance <= minDistance)
			{
				minDistance = distance;
				indexOf = m_vertexIndices[i];
			}
		}while(i < m_vertexIndices.size());
		return m_octree->getVertexData().at(indexOf);
	}
	return Vertex::null();;
}

int Octree::OctreeNode::indexOf(const Vertex& v)
{
	int index = -1;
	
	if (v == Vertex::null())
	{
		qDebug("OctreeNode::indexOf: Vertex cannot be null()");
		return false;
	}
	
	if (!isInVolume(v))
		return false;
	
	if (m_hasChildren)
	{
		if (m_childrenNodes.isEmpty())
			return index;
		
		int size = m_childrenNodes.size();
		for (int i = 0; i < size; ++i)
		{
			index = m_childrenNodes[i]->indexOf(v);
			if (index)
				break;
		}
	}
	else
	{
		
		if (m_vertexIndices.isEmpty())
			return index;
		
		int size = m_vertexIndices.size();
		for (int i = 0; i < size; ++i)
		{
			if (m_octree->getVertexData().at(m_vertexIndices[i]) == v)
			{
				index = m_vertexIndices[i];
				break;
			}
		}
	}
	
	return index;
}

bool Octree::OctreeNode::contains(const Vertex& v)
{	
	return indexOf(v) != -1;
}
	
bool Octree::OctreeNode::add(const Vertex& v)
{	
	if ( !isInVolume(v) )
		return false;
	
	if ( m_hasChildren )
	{
		foreach(OctreeNode* n, m_childrenNodes)
		{
			if ( n->add(v) )
				return true;
		}
	}
	else
	{
		if ( m_vertexIndices.size() < MaxVertex )
		{
			m_octree->getVertexData().append(v);
			int index = m_octree->getVertexData().size() - 1;
			m_vertexIndices.append(index);
			return true;
		}
		else
		{
			doPartition();
			foreach(OctreeNode* n, m_childrenNodes)
			{
				if ( n->add(v) )
					return true;
			}
		}
	}
	return false;
}
	
bool Octree::OctreeNode::remove(const Vertex& v)
{
	Q_UNUSED(v);
	
	return false;
}
	
void Octree::OctreeNode::doPartition()
{
	Vertex delta = (m_maximumCoords - m_minimumCoords) / 2.0;
	
	OctreeNode* node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	Vertex tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	tmp.setY(tmp.getY() + delta.getY());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setY(tmp.getY() + delta.getY());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	tmp.setY(tmp.getY() + delta.getY());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setY(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setX(tmp.getX() + delta.getX());
	tmp.setY(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	tmp = m_minimumCoords;
	tmp.setY(tmp.getY() + delta.getY());
	tmp.setZ(tmp.getZ() + delta.getZ());
	node = new OctreeNode(this);
	node->setMinimumCoords(m_minimumCoords);
	node->setMaximumCoords(m_minimumCoords + delta);
	m_childrenNodes.append(node);
	
	m_hasChildren = true;
}

bool Octree::OctreeNode::isInVolume(const Vertex& v)
{
	float x = v.getX();
	float y = v.getY();
	float z = v.getZ();
	if ( x < m_minimumCoords.getX() || x >= m_maximumCoords.getX()
		 || y < m_minimumCoords.getY() || y >= m_maximumCoords.getY()
		 || z < m_minimumCoords.getZ() || z >= m_maximumCoords.getZ())
	{
		return false;
	}
	
	return true;
}

Octree::Octree()
: m_rootNode(new OctreeNode(this))
{
	m_rootNode->setMinimumCoords(Vertex(-1000.0, -1000.0, -1000.0) );
	m_rootNode->setMaximumCoords(Vertex(1000.0, 1000.0, 1000.0) );
}

Octree::~Octree()
{
}
	
Vertex& Octree::operator[](int index)
{
	return m_vertexData[index];
}

const Vertex& Octree::operator[](int index) const
{
	return m_vertexData[index];
}
	
bool Octree::containsPoint(const Vertex& v) const
{
	return m_rootNode->contains(v);
}
	
void Octree::addPoint(const Vertex& v)
{
	m_rootNode->add(v);
}

void Octree::removePoint(const Vertex& v)
{
	Q_UNUSED(v);
	// TODO: Implement
}

void removePoint(int index)
{
	Q_UNUSED(index);
	// TODO: Implement
}

QVector<Vertex>& Octree::getVertexData()
{
	return m_vertexData;
}
*/