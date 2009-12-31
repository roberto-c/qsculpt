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
#include <QColor>
#include <QMultiHash>
#include "Point3D.h"
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

	bool contains(const Point3D& point);

	Point3D& at(int index);

	const Point3D& at(int index) const;

	int insert(const Point3D& point);

	void append(const Point3D& point);

	int indexOf(const Point3D& point);

	void clear();

	void remove(int index);
	
	void addFaceReference(int index, int faceIndex);

	void setVertex(int index, const Point3D& v);

	QVector<int>& getFaceReference(int index);

	const QVector<int>& getFaceReference(int index) const;

	GLfloat* getData() ;
	
private:
	PointContainer(){}

	// undefined: class does not make copies of it self
	PointContainer(const PointContainer&);
	PointContainer operator=(const PointContainer&);

	Octree<Point3D> m_pointList;
	QVector<QVector<int> > m_faceReference;
	QHash<Point3D, int> m_pointHash;

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
bool PointContainer::contains(const Point3D& point) {
	return m_pointHash.contains(point);
}

inline
Point3D& PointContainer::at(int index)  {
	return m_pointList[index];
}

inline
const Point3D& PointContainer::at(int index) const {
	return m_pointList[index];
}

inline
int PointContainer::insert(const Point3D& point) {
	m_pointList.append(point);
	int index =  m_pointList.size() -1;
	m_pointHash.insert(point,index);
	m_faceReference.append(QVector<int>());
	return index;
}

inline
void PointContainer::append(const Point3D& point) {
	m_pointList.append(point);
	m_pointHash.insert(point, m_pointList.size() -1);
	m_faceReference.append(QVector<int>());
}

inline
int PointContainer::indexOf(const Point3D& point) {
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
void PointContainer::setVertex(int index, const Point3D& v) {
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
}


#endif

