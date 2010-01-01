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
#include "StdAfx.h"
#include "Object3D.h"

#include <QColor>
#include <QtOpenGL>
#include <QVector>
#include <math.h>
#include <Eigen/Core>

#include "Box.h"
#include "Point3D.h"
#include "Scene.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

Object3D::Object3D()
    : IObject3D(),
    m_scene(NULL),
    //m_drawingMode(Wireframe),
    m_color(Qt::white),
    m_boundingBoxColor(Qt::red),
    m_minX(0.0),
    m_maxX(0.0),
    m_minY(0.0),
    m_maxY(0.0),
    m_minZ(0.0),
    m_maxZ(0.0),
    m_rotX(0.0),
    m_rotY(0.0),
    m_rotZ(0.0),
    m_selected(false),
    m_callListId(0),
    m_genereateCallList(true),
    m_currentResolutionLevel(0),
	m_hasChanged(true)
{
    initPoints();
    updateBoundingBox();
}

//Object3D::Object3D(const Object3D& cpy)
//    :   IObject3D(cpy),
//    m_scene(cpy.m_scene),
//    //m_drawingMode(cpy.m_drawingMode),
//    m_color(cpy.m_color),
//    m_boundingBoxColor(cpy.m_boundingBoxColor),
//    m_minX(cpy.m_minX),
//    m_maxX(cpy.m_maxX),
//    m_minY(cpy.m_minY),
//    m_maxY(cpy.m_maxY),
//    m_minZ(cpy.m_minZ),
//    m_maxZ(cpy.m_maxZ),
//    m_rotX(cpy.m_rotX),
//    m_rotY(cpy.m_rotY),
//    m_rotZ(cpy.m_rotZ),
//    m_selected(cpy.m_selected),
//    m_callListId(0),
//    m_genereateCallList(true),
//    m_currentResolutionLevel(cpy.m_currentResolutionLevel),
//    //m_pointList(cpy.m_pointList),
//    m_normalList(cpy.m_normalList)
//    //m_faceList(cpy.m_faceList)
//{
//	// TODO: IObject3D copy constructor broken
//}

Object3D::~Object3D()
{
}

void Object3D::initPoints()
{
    //qDebug("Object3D::initPoints()");
    FaceContainer* faceContainer = new FaceContainer;
    faceContainer->reserve(100000);
    m_faceList.append(faceContainer);

    EdgeContainer* edgeContainer = new EdgeContainer;
    edgeContainer->reserve(100000);
    m_edgeList.append(edgeContainer);

    m_pointList.clear();
	m_pointList.reserve(100000);
	m_normalList.reserve(100000);
	//m_faceList.reserve(100000);
	//m_edgeList.reserve(100000);
}

void Object3D::setScene(Scene* scene)
{
    m_scene = scene;
}

Scene* Object3D::getScene() const
{
    return m_scene;
}

Point3 Object3D::getPosition() const
{
  return m_position;
}

void Object3D::displace(const Point3& delta)
{
	m_position = m_position + delta;
	emit positionChanged(m_position.x(), m_position.y(), m_position.z());
}

void Object3D::getPosition(float *x, float *y, float *z) const
{
    if (x) *x = m_position.x();
    if (y) *y = m_position.y();
    if (z) *z = m_position.z();
}

void Object3D::rotate(float rotX, float rotY, float rotZ)
{
    m_rotX = rotX;
    m_rotY = rotY;
    m_rotZ = rotZ;
}

void Object3D::setPosition(float x, float y, float z)
{
	m_position.x() = x;
	m_position.y() = y;
	m_position.z() = z;

	emit positionChanged(x, y, z);
}

void Object3D::setPosition(const Point3& position)
{
	m_position = position;
	emit positionChanged(m_position.x(), m_position.y(), m_position.z());
}

float Object3D::getDepth()
{
    return m_maxZ - m_minZ;
}

float Object3D::getHeight()
{
    return m_maxY - m_minY;
}

float Object3D::getWidth()
{
    return m_maxX - m_minX;
}

void Object3D::setDepth(float value)
{
    //scale(1.0, 1.0, value / m_depth);
    //m_depth = value;
    //updateBoundingBox();
    Q_UNUSED(value);
}

void Object3D::setHeight(float value)
{
    //scale(1.0, value / m_height, 1.0);
    //m_height = value;
    //updateBoundingBox();
    Q_UNUSED(value);
}

void Object3D::setWidth(float value)
{
    //scale(value / m_width, 1.0, 1.0);
    //m_width = value;
    //updateBoundingBox();
    Q_UNUSED(value);
}

void Object3D::setColor(const QColor& color)
{
    m_color = color;
}

QColor Object3D::getColor() const
{
    return m_color;
}

void Object3D::setSelected(bool val)
{
    m_selected = val;
}

bool Object3D::isSelected() const
{
    return m_selected;
}

void Object3D::setBoundingBoxColor(const QColor& color)
{
    m_boundingBoxColor = color;
}

QColor Object3D::getBoundingBoxColor() const
{
    return m_boundingBoxColor;
}

int Object3D::addVertex(const Point3& point)
{
    float x = point[0];
    float y = point[1];
    float z = point[2];

    m_minX = MIN(x , m_minX);
    m_minY = MIN(y , m_minY);
    m_minZ = MIN(z , m_minZ);

    m_maxX = MAX(x , m_maxX);
    m_maxY = MAX(y , m_maxY);
    m_maxZ = MAX(z , m_maxZ);

    m_pointList.append(point);
    m_normalList.append(Vector3());
    return m_pointList.size()-1;
}

void Object3D::removeVertex(int id)
{
    if (id >=0 )
        m_pointList.remove(id);
}

Point3& Object3D::getVertex(int index)
{
    return m_pointList.at(index);
}

Vector3& Object3D::getNormalAtPoint(int index)
{
	return m_normalList[index];
}

const Vector3& Object3D::getNormalAtPoint(int index) const
{
	return m_normalList[index];
}

int Object3D::addEdge(const Edge& edge)
{
	int indexOf = -1; 
	if ((indexOf = m_edgeList[m_currentResolutionLevel]->indexOf(edge)) == -1)
	{
		indexOf = m_edgeList[m_currentResolutionLevel]->insert(edge);
	}
	return indexOf;
}

int Object3D::addEdge(int v1, int v2)
{
	Edge edge(v1, v2);
	return addEdge(edge);
}

int Object3D::addFace(const QVector<int>& vertexIndexList)
{
    //qDebug("Object3D::addFace");
    Face t(vertexIndexList);
	int numFaceVertices = t.point.size();
	int numVertices = m_pointList.size();
	
	bool isValid = true;
	for (int i = 0; i < numFaceVertices; ++i)
	{
//		qDebug()<<t.point[i];
		if (t.point[i] >= numVertices || t.point[i] < 0)
			isValid = false;
	}
	
    if (isValid)
    {
//		qDebug("Object3D::addFace: get face and edge lists");
        FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
        EdgeContainer& edgeList = *m_edgeList[m_currentResolutionLevel];

        int faceIndex = faceList.size();
        for (int i = 0; i < numFaceVertices; ++i)
        {
            m_pointList.addFaceReference(t.point[i], faceIndex);
        }

//		qDebug("Object3D::addFace: add edges");
		Edge edge;
		int edgeIndex = -1;
		for (int i = 0; i < numFaceVertices; i++)
		{
			edge.point1 = t.point.at(i);
			edge.point2 = t.point.at((i + 1) %numFaceVertices);
			if ((edgeIndex = edgeList.indexOf(edge)) == -1)
			{
//				qDebug("Object3D::addFace: edge not found");
				edgeIndex = edgeList.insert(edge);
			}
			edgeList.addFaceReference(edgeIndex, faceIndex);
			t.edge.append(edgeIndex);
		}
        faceList.append(t);
		for (int i = 0; i < numFaceVertices; i++)
		{
			adjustPointNormal(t.point[i]);
		}
//		qDebug("Object3D::addFace: Face added");
		return faceIndex;
    }
    qDebug("Object3D::addFace: Face is not valid.");
    return -1;
}

void Object3D::replaceFace(int faceIndex, const QVector<int>& vertexIndexList)
{
    //qDebug("Object3D::replaceFace");
	int numFaceVertices =vertexIndexList.size();
	int numVertices = m_pointList.size();
	bool isValid = true;
	for (int i = 0; i < numFaceVertices; ++i)
	{
		if (vertexIndexList[i] >= numVertices || vertexIndexList[i] < 0)
			isValid = false;
	}
	if (isValid)
	{
		FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
		Face& t = faceList[faceIndex];
		t.setPoints(vertexIndexList);
		t.edge.clear();

        EdgeContainer& edgeList = *m_edgeList[m_currentResolutionLevel];
		Edge edge;
		int vertexCount = t.point.size();
		int edgeIndex = -1;
		for (int i = 0; i < vertexCount; i++)
		{
			edge.point1 = t.point.at(i);
			edge.point2 = t.point.at((i + 1) %vertexCount);
			edgeIndex = edgeList.indexOf(edge);
			if (edgeIndex == -1)
			{
				edgeList.append(edge);
				edgeIndex = edgeList.size() - 1;
			}
			edgeList.addFaceReference(edgeIndex, faceIndex);
			faceList[faceIndex].edge.append(edgeIndex);
		}
		int numFaceVertices = t.point.size();
        for (int i = 0; i < numFaceVertices; ++i)
        {
            m_pointList.addFaceReference(t.point[i], faceIndex);
            adjustPointNormal(t.point[i]);
        }
    }
}

void Object3D::removeFace( int id)
{
	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    Face& f = faceList[id];
    int pointCount = f.point.size();
    for (int i = 0; i < pointCount; ++i)
    {
        int index = m_pointList.getFaceReference(f.point[i]).indexOf(id);
        if (index >= 0)
            m_pointList.addFaceReference(f.point[i], index);
    }
    m_faceList.remove(id);
    // TODO: remove Edges not used from edges list
}

void Object3D::drawBoundingBox()
{
    GLboolean lightEnabled = GL_FALSE;
    lightEnabled = glIsEnabled(GL_LIGHTING);

    if (lightEnabled == GL_TRUE)
        glDisable(GL_LIGHTING);

    glColor3f(0.0, 1.0, 0.0);

    float minX = m_minX - 0.1;
    float maxX = m_maxX + 0.1;
    float minY = m_minY - 0.1;
    float maxY = m_maxY + 0.1;
    float minZ = m_minZ - 0.1;
    float maxZ = m_maxZ + 0.1;

    glColor3f(0.0, 1.0, 0.0);

    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, minZ);
    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, maxY, minZ);
    glVertex3f(minX, maxY, minZ);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ);
    glVertex3f(minX, minY, maxZ);

    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, minY, maxZ);

    glVertex3f(maxX, maxY, minZ);
    glVertex3f(maxX, maxY, maxZ);

    glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();

    if (lightEnabled == GL_TRUE)
        glEnable(GL_LIGHTING);
}

void Object3D::updateBoundingBox()
{
	//TODO: Clean this up
#define setPoint(p, px, py, pz) p.x() = (px); p.y() = py; p.z() = pz
    float hw = getWidth() / 2;
    float hh = getHeight() / 2;
    float hd = getDepth() / 2;

    setPoint(m_boundingBoxVert[0], -hw - 0.2,-hh - 0.2,-hd - 0.2);
    setPoint(m_boundingBoxVert[1], hw + 0.2,-hh - 0.2,-hd - 0.2);
    setPoint(m_boundingBoxVert[2], hw + 0.2, hh + 0.2,-hd - 0.2);
    setPoint(m_boundingBoxVert[3], -hw - 0.2, hh + 0.2,-hd - 0.2);

    setPoint(m_boundingBoxVert[4], -hw - 0.2,-hh - 0.2, hd + 0.2);
    setPoint(m_boundingBoxVert[5],  hw + 0.2,-hh - 0.2, hd + 0.2);
    setPoint(m_boundingBoxVert[6],  hw + 0.2, hh + 0.2, hd + 0.2);
    setPoint(m_boundingBoxVert[7], -hw - 0.2, hh + 0.2, hd + 0.2);
#undef setPoint
}

void Object3D::scale(float xfactor, float yfactor, float zfactor)
{
    int size = m_pointList.size();
    float x, y, z;
    for (int i = 0; i < size; i++)
    {
		x = m_pointList.at(i).x();
		y = m_pointList.at(i).y();
		z = m_pointList.at(i).z();
        //m_pointList.at(i).getPoint(x, y, z);
        x = (xfactor == 1.0) ? x : x * xfactor;
        y = (yfactor == 1.0) ? y : y * yfactor;
        z = (zfactor == 1.0) ? z : z * zfactor;
		Point3 v(x, y, z);
        m_pointList.setVertex(i, v);
    }
}

int Object3D::getFaceIndexAtPoint(const Point3& p) const
{
    int closesPointIndex = getClosestPointAtPoint(p);
    Face face;
    Point3 point;
    int faceCount = m_pointList.getFaceReference(closesPointIndex).size();
    int indexOf = -1;
    float distance = 0.0, minDistance = 0.0;

    if (faceCount > 0 )
    {
    	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];

        indexOf = 0;
        face = faceList[0];

        for (int j = 0; j < face.point.size(); ++j)
        {
            point = m_pointList.at(face.point[j]);
            distance += fabs((p - point).norm());
        }
        minDistance = distance;
        //qDebug(" %f ", distance);
        for (int i = 1; i < faceCount; i ++)
        {
            distance = 0.0;
            face = faceList[m_pointList.getFaceReference(closesPointIndex).at(i)];
            for (int j = 0; j < face.point.size(); ++j)
            {
                point = m_pointList.at(face.point[j]);
                distance += fabs((p - point).norm());
            }

            if ( distance < minDistance )
            {
                minDistance = distance;
                indexOf = i;
                //qDebug(" %f ", distance);
            }
            //qDebug("%f", distance);
        }
    }

    return indexOf;
}

int Object3D::getClosestPointAtPoint(const Point3 &p) const
{
    int pointCount = m_pointList.size();
    int indexOf = -1;
    float distance = 0.0, minDistance = 0.0;

    if (pointCount > 0 )
    {
        indexOf = 0;
        Point3 point = m_pointList.at(0);

        distance = fabs((p - point).norm());
        minDistance = distance;
        //qDebug(" %f ", distance);
        for (int i = 1; i < pointCount; i ++)
        {
            point = m_pointList.at(i);
            distance = fabs((p - point).norm());
            if ( distance < minDistance )
            {
                minDistance = distance;
                indexOf = i;
                //qDebug(" %f ", distance);
            }
            //qDebug("%f", distance);
        }
    }

    return indexOf;
}

QVector<int> Object3D::getPointsInRadius(const Point3 &p, float radius) const
{
	//qDebug() << "Object3D::getPointsInRadius()";
    QVector<int> results;

    int pointCount = m_pointList.size();
    float distance = 0.0;

    //qDebug() << "Point count = " << pointCount;
    if (pointCount > 0 )
    {
        for (int i = 0; i < pointCount; i ++)
        {
            const Point3& point = m_pointList.at(i);
            distance = fabs((p - point).norm());
            if ( distance < radius )
            {
                results.append(i);
                //qDebug(" %f ", distance);
            }
            //qDebug("%f", distance);
        }
    }

    return results;
}

Vector3 Object3D::computeFaceNormal(int index)
{
	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    return faceList.size() > index ? computeFaceNormal(faceList[index]) : Vector3();
}

Vector3 Object3D::computeFaceNormal(Face &face)
{
    int lastPoint = face.point.size() - 1;
    Point3 v1 = m_pointList.at(face.point[1])
        - m_pointList.at(face.point[0]);
    Point3 v2 = m_pointList.at(face.point[lastPoint])
        - m_pointList.at(face.point[0]);

    Vector3 res = v1.cross( v2);
    res.normalize();

    return res;
}

void Object3D::adjustPointNormal(int index)
{
    Vector3 res;
	
    int numFaces = m_pointList.getFaceReference(index).size();
    for (int i = 0; i < numFaces; i++)
    {
        res = res + computeFaceNormal(m_pointList.getFaceReference(index).at(i));
    }

    res = res / (float)numFaces;
    res.normalize();

	m_normalList[index] = res;
}

void Object3D::computeAllNormals()
{
    qDebug("computeAllNormals: Not Implemented");
}

const NormalContainer& Object3D::getNormalList() const
{
    return m_normalList;
}

const PointContainer& Object3D::getPointList() const
{
    return m_pointList;
}

const FaceContainer& Object3D::getFaceList() const
{
    return *m_faceList[m_currentResolutionLevel];
}

NormalContainer& Object3D::getNormalList()
{
    return m_normalList;
}

PointContainer& Object3D::getPointList()
{
    return m_pointList;
}

FaceContainer& Object3D::getFaceList()
{
    return *m_faceList[m_currentResolutionLevel];
}

EdgeContainer& Object3D::getEdgeList()
{
	return *m_edgeList[m_currentResolutionLevel];
}

const EdgeContainer& Object3D::getEdgeList() const
{
	return *m_edgeList[m_currentResolutionLevel];
}

void Object3D::lock() const
{
    m_mutex.lock();
}

void Object3D::unlock() const
{
    m_mutex.unlock();
}

void Object3D::addResolutionLevel()
{
	//TODO: Implement addResolutionLevel
}

void Object3D::removeResolutionLevel(int level)
{
	// TODO: Implement removeResolutionLevel
	Q_UNUSED(level);
}

void Object3D::setWorkingResolutionLevel(int level)
{
	// TODO: Implement setWorkingResolutionLevel
	Q_UNUSED(level);
}

int Object3D::getWorkingResolutionLevel()
{
	// TODO: Implement getWorkingResolutionLevel
	return m_currentResolutionLevel;
}
