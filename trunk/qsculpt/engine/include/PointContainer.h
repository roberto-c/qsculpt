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
#ifndef POINTCONTAINER_H_
#define POINTCONTAINER_H_

#include <QtOpenGL>
#include <QVector>
#include <QMultiHash>
#include <QColor>
#include "octree.h"

//struct Point
//{
//    Vertex vertex;
//    QColor color;
//    QVector<int> faceRef;
//
//    Point() : color(Qt::gray) {}
//    Point(float x, float y, float z) : vertex(x, y, z), color(Qt::gray){}
//    Point(const Vertex& v) : vertex(v), color(Qt::gray) {}
//    Point(const Point& cpy) : vertex(cpy.vertex), color(cpy.color), faceRef(cpy.faceRef) {}
//
//    operator Vertex() {
//        return vertex;
//    }
//
//    operator Vertex() const {
//        return vertex;
//    }
//
//    bool operator==(const Vertex& v) const {
//        return vertex == v;
//    }
//
//    bool operator==(const Point& p) const {
//        return vertex == p.vertex;
//    }
//};

class PointContainer
{
public:
	void reserve(int size) {
		m_pointList.reserve(size);
		m_faceReference.reserve(size);
		m_pointHash.reserve(size);
	}

	int size() const {
		return m_pointList.size();
	}

	bool contains(const Vertex& point) {
		return m_pointHash.contains(point);
	}

	Vertex& at(int index)  {
		return m_pointList[index];
	}

	const Vertex& at(int index) const {
		return m_pointList[index];
	}

	int insert(const Vertex& point) {
		m_pointList.append(point);
		int index =  m_pointList.size() -1;
		m_pointHash.insert(point,index);
		m_faceReference.append(QVector<int>());
		return index;
	}

	void append(const Vertex& point) {
		m_pointList.append(point);
		m_pointHash.insert(point, m_pointList.size() -1);
		m_faceReference.append(QVector<int>());
	}

	int indexOf(const Vertex& point) {
		if (!m_pointHash.contains(point))
			return -1;
		return m_pointHash.value(point);
	}

	void clear() {
		m_pointList.clear();
		m_pointHash.clear();
	}

	void remove(int index) {
		m_pointHash.remove(m_pointList.at(index));
		m_pointList.remove(index);
		m_faceReference.remove(index);
	}

	void addFaceReference(int index, int faceIndex) {
		m_faceReference[index].append(faceIndex);
	}

	void setVertex(int index, const Vertex& v) {
		m_pointHash.remove(m_pointList.at(index));
		m_pointList[index] = v;
		m_pointHash.insert(m_pointList[index], index);
	}

	QVector<int>& getFaceReference(int index) {
		return m_faceReference[index];
	}

	const QVector<int>& getFaceReference(int index) const {
		return m_faceReference[index];
	}

	inline GLfloat* getData() {
		return (GLfloat*)m_pointList.getData();
	};
	
private:
	PointContainer(){}

	// undefined: class does not make copies of it self
	PointContainer(const PointContainer&);
	PointContainer operator=(const PointContainer&);

	Octree<Vertex> m_pointList;
	QVector<QVector<int> > m_faceReference;
	QHash<Vertex, int> m_pointHash;

	friend class Object3D;
};

inline uint qHash(const Vertex& key)
{
	qint16 x = lrintf(key.getX() * 1000.0f);
	qint16 y = lrintf(key.getY() * 1000.0f);
	qint16 z = lrintf(key.getZ() * 1000.0f);
	quint64 d = (quint64)(x && 0xFFFF) << 32 | (quint64)(y & 0xFFFF) << 16 | (quint64)(z & 0xFFFF);
	return qHash(d);
}

#endif

