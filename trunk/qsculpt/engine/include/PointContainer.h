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
#include "Octree.h"

typedef QVector<int> PointIndexList;

/**
 *
 */
class PointContainer
{
public:
	void reserve(int size);
	
	int size() const;

	bool contains(const Vertex& point);

	Vertex& at(int index);

	const Vertex& at(int index) const;

	int insert(const Vertex& point);

	void append(const Vertex& point);

	int indexOf(const Vertex& point);

	void clear();

	void remove(int index);
	
	void addFaceReference(int index, int faceIndex);

	void setVertex(int index, const Vertex& v);

	QVector<int>& getFaceReference(int index);

	const QVector<int>& getFaceReference(int index) const;

	GLfloat* getData() ;
	
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

// Inline definitions
inline
void PointContainer::reserve(int size) {
	m_pointList.reserve(size);
	m_faceReference.reserve(size);
	m_pointHash.reserve(size);
}

inline
int PointContainer::size() const {
	return m_pointList.size();
}

inline
bool PointContainer::contains(const Vertex& point) {
	return m_pointHash.contains(point);
}

inline
Vertex& PointContainer::at(int index)  {
	return m_pointList[index];
}

inline
const Vertex& PointContainer::at(int index) const {
	return m_pointList[index];
}

inline
int PointContainer::insert(const Vertex& point) {
	m_pointList.append(point);
	int index =  m_pointList.size() -1;
	m_pointHash.insert(point,index);
	m_faceReference.append(QVector<int>());
	return index;
}

inline
void PointContainer::append(const Vertex& point) {
	m_pointList.append(point);
	m_pointHash.insert(point, m_pointList.size() -1);
	m_faceReference.append(QVector<int>());
}

inline
int PointContainer::indexOf(const Vertex& point) {
	if (!m_pointHash.contains(point))
		return -1;
	return m_pointHash.value(point);
}

inline
void PointContainer::clear() {
	m_pointList.clear();
	m_pointHash.clear();
}

inline
void PointContainer::remove(int index) {
	m_pointHash.remove(m_pointList.at(index));
	m_pointList.remove(index);
	m_faceReference.remove(index);
}

inline
void PointContainer::addFaceReference(int index, int faceIndex) {
	m_faceReference[index].append(faceIndex);
}

inline
void PointContainer::setVertex(int index, const Vertex& v) {
	m_pointHash.remove(m_pointList.at(index));
	m_pointList[index] = v;
	m_pointHash.insert(m_pointList[index], index);
}

inline
QVector<int>& PointContainer::getFaceReference(int index) {
	return m_faceReference[index];
}

inline
const QVector<int>& PointContainer::getFaceReference(int index) const {
	return m_faceReference[index];
}

inline
GLfloat* PointContainer::getData() {
	return (GLfloat*)m_pointList.getData();
};

inline
uint qHash(const Vertex& key)
{
	qint16 x = lrintf(key.getX() * 1000.0f);
	qint16 y = lrintf(key.getY() * 1000.0f);
	qint16 z = lrintf(key.getZ() * 1000.0f);
	quint64 d = (quint64)(x && 0xFFFF) << 32 | (quint64)(y & 0xFFFF) << 16 | (quint64)(z & 0xFFFF);
	return qHash(d);
}

#endif

