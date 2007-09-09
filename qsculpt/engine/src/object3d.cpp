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
#include "stdafx.h"
#include "object3d.h"

#include <QColor>
#include <QtOpenGL>
#include <QVector>
#include <math.h>

#include "box.h"
#include "point3d.h"
#include "scene.h"


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
    m_currentResolutionLevel(0)
{
    initPoints();
    updateBoundingBox();
}

Object3D::Object3D(const Object3D& cpy)
    :   IObject3D(cpy),
    m_scene(cpy.m_scene),
    //m_drawingMode(cpy.m_drawingMode),
    m_color(cpy.m_color),
    m_boundingBoxColor(cpy.m_boundingBoxColor),
    m_minX(cpy.m_minX),
    m_maxX(cpy.m_maxX),
    m_minY(cpy.m_minY),
    m_maxY(cpy.m_maxY),
    m_minZ(cpy.m_minZ),
    m_maxZ(cpy.m_maxZ),
    m_rotX(cpy.m_rotX),
    m_rotY(cpy.m_rotY),
    m_rotZ(cpy.m_rotZ),
    m_selected(cpy.m_selected),
    m_callListId(0),
    m_genereateCallList(true),
    m_currentResolutionLevel(cpy.m_currentResolutionLevel),
    //m_pointList(cpy.m_pointList),
    m_normalList(cpy.m_normalList)
    //m_faceList(cpy.m_faceList)
{
	// TODO: IObject3D copy constructor broken
}

Object3D::~Object3D()
{
}

void Object3D::initPoints()
{
    qDebug("Object3D::initPoints()");
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

Point3D Object3D::getPosition() const
{
  return m_position;
}

void Object3D::displace(const Point3D& delta)
{
  m_position = m_position + delta;
}

void Object3D::getPosition(float *x, float *y, float *z) const
{
    if (x) *x = m_position.getX();
    if (y) *y = m_position.getY();
    if (z) *z = m_position.getZ();
}

void Object3D::rotate(float rotX, float rotY, float rotZ)
{
    m_rotX = rotX;
    m_rotY = rotY;
    m_rotZ = rotZ;
}

void Object3D::setPosition(float x, float y, float z)
{
  m_position.setX(x);
  m_position.setY(y);
  m_position.setZ(z);
}

void Object3D::setPosition(const Point3D& position)
{
  m_position = position;
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

const QColor Object3D::getColor()
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

int Object3D::addVertex(const Vertex& point)
{
    float x = point.getX();
    float y = point.getY();
    float z = point.getZ();

    m_minX = MIN(x , m_minX);
    m_minY = MIN(y , m_minY);
    m_minZ = MIN(z , m_minZ);

    m_maxX = MAX(x , m_maxX);
    m_maxY = MAX(y , m_maxY);
    m_maxZ = MAX(z , m_maxZ);

    m_pointList.append(point);
    m_normalList.append(Normal());
    return m_pointList.size();
}

void Object3D::removeVertex(int id)
{
    if (id >=0 )
        m_pointList.remove(id);
}

Vertex& Object3D::getVertex(int index)
{
    return m_pointList.at(index).vertex;
}

Normal& Object3D::getNormalAtPoint(int index)
{
    const Point p = m_pointList.at(index);

    //qDebug("size: %d", p.faceRef.size());
    if ( !p.faceRef.isEmpty() )
    {
    	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
        const Face& t = faceList[p.faceRef[0]];
        int numPoints = t.point.size();
        for (int i = 0; i < numPoints; i++)
        {
            if (t.point[i] == index)
                return m_normalList[t.normal[i]];
        }

        qDebug("Normal not found!!!");
        qDebug("Index: %d", index);
        qDebug() << "Triangle Ref:";
        for (int i = 0; i < numPoints; i++)
        {
            qDebug() << " " << t.point[i];
        }
        qDebug() << endl;
    }
    else
        qDebug("faceReference Empty!!!");

    return const_cast<Normal&>(Normal::null());
}

const Normal& Object3D::getNormalAtPoint(int index) const
{
    const Point& p = m_pointList.at(index);

    if ( !p.faceRef.isEmpty() )
    {
    	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
        const Face& t = faceList[p.faceRef[0]];
        int numPoints = t.point.size();
        for (int i = 0; i < numPoints; i++)
        {
            if (t.point[i] == index)
                return m_normalList.at(t.normal[i]);
        }
    }
    qDebug("Normal not found!!!");
    return Normal::null();
}

int Object3D::addFace(const QVector<int>& vertexIndexList)
{
    //qDebug("addFace");
    Face t(vertexIndexList);

    if (t.isValid())
    {
        Normal normal;
        normal = computeFaceNormal( t );

        for (int i = 0; i < t.point.size(); ++i)
        {
            m_normalList.append(normal);
            t.normal[i] = t.point.at(i);
        }
        FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
        EdgeContainer& edgeList = *m_edgeList[m_currentResolutionLevel];

        faceList.append(t);

        int triangleIndex = faceList.size() - 1;
        int pointSize = t.point.size();
        for (int i = 0; i < pointSize; ++i)
        {
            m_pointList.addFaceReference(t.point[i], triangleIndex);
            adjustPointNormal(t.point[i]);
        }

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
			edgeList.addFaceReference(edgeIndex, triangleIndex);
			faceList[triangleIndex].edge.append(edgeIndex);
		}

        return triangleIndex;
    }
    qDebug("Face added is not valid.");
    return -1;
}

void Object3D::replaceFace(int faceIndex, const QVector<int>& vertexIndexList)
{
    //qDebug("addFace");
	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    Face& t = faceList[faceIndex];
	t.setPoints(vertexIndexList);
	t.edge.clear();
    if (t.isValid())
    {
        Normal normal;
        normal = computeFaceNormal( t );

        for (int i = 0; i < t.point.size(); ++i)
        {
            m_normalList.append(normal);
            t.normal[i] = t.point.at(i);
        }

        int pointSize = t.point.size();
        for (int i = 0; i < pointSize; ++i)
        {
            m_pointList.addFaceReference(t.point[i], faceIndex);
            adjustPointNormal(t.point[i]);
        }

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
    }
	else
		qDebug("Face replaced is not valid.");
}

void Object3D::removeFace( int id)
{
	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    Face& f = faceList[id];
    int pointCount = f.point.size();
    for (int i = 0; i < pointCount; ++i)
    {
        int index = m_pointList.at(f.point[i]).faceRef.indexOf(id);
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
    float hw = getWidth() / 2;
    float hh = getHeight() / 2;
    float hd = getDepth() / 2;

    m_boundingBoxVert[0].setPoint(-hw - 0.2,-hh - 0.2,-hd - 0.2);
    m_boundingBoxVert[1].setPoint( hw + 0.2,-hh - 0.2,-hd - 0.2);
    m_boundingBoxVert[2].setPoint( hw + 0.2, hh + 0.2,-hd - 0.2);
    m_boundingBoxVert[3].setPoint(-hw - 0.2, hh + 0.2,-hd - 0.2);

    m_boundingBoxVert[4].setPoint(-hw - 0.2,-hh - 0.2, hd + 0.2);
    m_boundingBoxVert[5].setPoint( hw + 0.2,-hh - 0.2, hd + 0.2);
    m_boundingBoxVert[6].setPoint( hw + 0.2, hh + 0.2, hd + 0.2);
    m_boundingBoxVert[7].setPoint(-hw - 0.2, hh + 0.2, hd + 0.2);
}

void Object3D::scale(float xfactor, float yfactor, float zfactor)
{
    int size = m_pointList.size();
    float x, y, z;
    for (int i = 0; i < size; i++)
    {
        m_pointList.at(i).vertex.getPoint(x, y, z);
        x = (xfactor == 1.0) ? x : x * xfactor;
        y = (yfactor == 1.0) ? y : y * yfactor;
        z = (zfactor == 1.0) ? z : z * zfactor;
		Vertex v(x, y, z);
        m_pointList.setVertex(i, v);
    }
}

int Object3D::getFaceIndexAtPoint(const Point3D& p) const
{
    int closesPointIndex = getClosestPointAtPoint(p);
    Face face;
    Point3D point;
    int faceCount = m_pointList.at(closesPointIndex).faceRef.size();
    int indexOf = -1;
    float distance = 0.0, minDistance = 0.0;

    if (faceCount > 0 )
    {
    	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];

        indexOf = 0;
        face = faceList[0];

        for (int j = 0; j < face.point.size(); ++j)
        {
            point = m_pointList.at(face.point[j]).vertex;
            distance += fabs((p - point).length());
        }
        minDistance = distance;
        //qDebug(" %f ", distance);
        for (int i = 1; i < faceCount; i ++)
        {
            distance = 0.0;
            face = faceList[m_pointList.at(closesPointIndex).faceRef[i]];
            for (int j = 0; j < face.point.size(); ++j)
            {
                point = m_pointList.at(face.point[j]).vertex;
                distance += fabs((p - point).length());
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

int Object3D::getClosestPointAtPoint(const Point3D &p) const
{
    int pointCount = m_pointList.size();
    int indexOf = -1;
    float distance = 0.0, minDistance = 0.0;

    if (pointCount > 0 )
    {
        indexOf = 0;
        Point3D point = m_pointList.at(0).vertex;

        distance = fabs((p - point).length());
        minDistance = distance;
        //qDebug(" %f ", distance);
        for (int i = 1; i < pointCount; i ++)
        {
            point = m_pointList.at(i).vertex;
            distance = fabs((p - point).length());
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

QVector<int> Object3D::getPointsInRadius(const Point3D &p, float radius) const
{
    QVector<int> results;

    int pointCount = m_pointList.size();
    float distance = 0.0;

    if (pointCount > 0 )
    {
        for (int i = 0; i < pointCount; i ++)
        {
            const Point3D& point = m_pointList.at(i).vertex;
            distance = fabs((p - point).length());
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

Point3D Object3D::computeFaceNormal(int index)
{
	FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    return faceList.size() > index ? computeFaceNormal(faceList[index]) : Point3D();
}

Point3D Object3D::computeFaceNormal(Face &face)
{
    int lastPoint = face.point.size() - 1;
    Point3D v1 = m_pointList.at(face.point[1]).vertex
        - m_pointList.at(face.point[0]).vertex;
    Point3D v2 = m_pointList.at(face.point[lastPoint]).vertex
        - m_pointList.at(face.point[0]).vertex;

    Point3D res = v1.crossProduct( v2);
    res.normalize();

    return res;
}

void Object3D::adjustPointNormal(int index)
{
    Normal res;

    const Point& p = m_pointList.at(index);
    int numFaces = p.faceRef.size();
    for (int i = 0; i < numFaces; i++)
    {
        res = res + computeFaceNormal(p.faceRef[i]);
    }

    res = res / (float)numFaces;
    res.normalize();

    if (index == 0)
        qDebug("First point: res: %s numFaces: %d", qPrintable(res.toString()), numFaces);

    FaceContainer& faceList = *m_faceList[m_currentResolutionLevel];
    for (int i = 0; i < numFaces; i++)
    {
        Face& t = faceList[p.faceRef[i]];
        int numFacePoints = t.point.size();
        for (int j = 0; j < numFacePoints; ++j)
        {
            if (index == t.point[j])
            {
                m_normalList[t.normal[j]] = res;
            }
        }
    }
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

void Object3D::lock()
{
    m_mutex.lock();
}

void Object3D::unlock()
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
