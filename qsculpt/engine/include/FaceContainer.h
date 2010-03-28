/***************************************************************************
*   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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

#ifndef FACECONTAINER_H
#define FACECONTAINER_H

#include "Vertex.h"
#include "Face.h"

class FaceContainer
{
public:
	void reserve(int size) {
		m_faceList.reserve(size);
		m_faceHash.reserve(size);
	}
	
	int size() const {
		return m_faceList.size();
	}
	
	bool contains(const Face& face) {
		return m_faceHash.contains(face);
	}
	
	Face& operator[](int index) {
		return m_faceList[index];
	}
	
	const Face& operator[](int index) const {
		return m_faceList[index];
	}
	
	const Face& at(int index) const {
		return m_faceList.at(index);
	}
	
	int insert(const Face& face) {
		m_faceList.append(face);
		int index =  m_faceList.size() -1;
		m_faceHash.insert(face,index);
		for (int i = 0; i < face.point.size(); ++i)
		{
			m_pointIndices.append(face.point[i]);
		}
		return index;
	}
	
	void append(const Face& face) {
		m_faceList.append(face);
		m_faceHash.insert(face, m_faceList.size() -1);
		for (int i = 0; i < face.point.size(); ++i)
		{
			m_pointIndices.append(face.point[i]);
		}
	}
	
	void remove(int index) {
		m_faceHash.remove(m_faceList[index]);
		m_faceList.remove(index);
	}
	
	int indexOf(const Face& face) {
		if (!m_faceHash.contains(face))
			return -1;
		return m_faceHash.value(face);
	}
		
	void setMidPointReference(int index, int midPointIndex) {
		m_faceList[index].midPoint = midPointIndex;
	}
	
	void addEdgeReference(int index, int edgeIndex) {
		if (m_faceList[index].edge.indexOf(edgeIndex) == -1)
			m_faceList[index].edge.append(edgeIndex);
	}
	
	GLuint* getData() {
		return m_pointIndices.data();
	}
	
private:
	FaceContainer(){}
	
	// undefined: class does not make copies of itself
	FaceContainer(const FaceContainer&);
	FaceContainer operator=(const FaceContainer&);
	
	Octree<Face> m_faceList;
	QHash<Face, int> m_faceHash;
	QVector<GLuint> m_pointIndices;
	
	friend class Object3D;
	friend class Subdivision;
};

inline uint qHash(const Face& key)
{
	return qHash( key.hashValue );
}

#endif
