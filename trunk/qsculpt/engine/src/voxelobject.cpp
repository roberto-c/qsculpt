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
//#include "stdafx.h"
#include "voxelobject.h"
#include <QColor>
#include <QtOpenGL>
#include <math.h>

//#include "box.h"
#include "point3d.h"
#include "scene.h"
#include "octree.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


VoxelObject::VoxelObject() 
	:	IObject3D(),
		m_scene(NULL),
		m_drawingMode(Wireframe),
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
		m_showBoundingBox(false),
		m_callListId(0),
		m_regCallList(true)
{
	m_callListId = glGenLists(1);
	if (m_callListId == 0)
		qDebug("glGenList failed.");
	else
		qDebug("m_callListId = %d", m_callListId);
}

VoxelObject::VoxelObject(const VoxelObject& cpy)
	:   IObject3D(cpy),
		m_scene(cpy.m_scene),
		m_drawingMode(cpy.m_drawingMode),
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
		m_showBoundingBox(cpy.m_showBoundingBox),
		m_callListId(0),
		m_regCallList(true),
		m_pointList(cpy.m_pointList),
		m_pointMap(cpy.m_pointMap)
{
	m_callListId = glGenLists(1);
	if (m_callListId == 0)
		qDebug("glGenList failed.");
	else
		qDebug("m_callListId = %d", m_callListId);
}

VoxelObject::~VoxelObject()
{
	glDeleteLists(m_callListId, 1);
}

/******************************************************************************
* IObject3D interface
*/
void VoxelObject::setScene(Scene* scene)
{
	m_scene = scene;
}

Scene* VoxelObject::getScene() const
{
	return m_scene;
}

Point3D VoxelObject::getPosition() const
{
	return m_position;
}

void VoxelObject::displace(Point3D delta)
{
	m_position = m_position + delta;
}

void VoxelObject::getPosition(float *x, float *y, float *z) const
{
	if (x) *x = m_position.getX();
	if (y) *y = m_position.getY();
	if (z) *z = m_position.getZ();
}

void VoxelObject::setPosition(float x, float y, float z)
{
	m_position.setPoint(x,y,z);
}

void VoxelObject::setPosition(Point3D position)
{
	m_position = position;
}

void VoxelObject::rotate(float rotX, float rotY, float rotZ)
{
	m_rotX = rotX;
	m_rotY = rotY;
	m_rotZ = rotZ;
}

void VoxelObject::draw()
{
	
	if (m_regCallList) {
		glPushMatrix();
		glLoadIdentity();
		glNewList(m_callListId, GL_COMPILE);
		int pointCount = m_pointList.size();
		glBegin(GL_QUADS);
		for(int i = 0; i < pointCount; ++i) {
			drawCube(m_pointList[i], facesToDraw(m_pointList[i]));
		}
		glEnd();
		glEndList();
		m_regCallList = false;
		glPopMatrix();
	}
	
	glPushMatrix();
	
    glRotated(m_rotX, 1, 0, 0);
    glRotated(m_rotY, 0, 1, 0);
    glRotated(m_rotZ, 0, 0, 1);
    glTranslatef(m_position.getX(), m_position.getY(), m_position.getZ());
    
    if (m_showBoundingBox)
        drawBoundingBox();
	qDebug("m_callListId=%d", m_callListId);
	glCallList(m_callListId);
	
	glPopMatrix();
}

float VoxelObject::getDepth()
{
	return m_maxZ - m_minZ;
}

float VoxelObject::getHeight()
{
	return m_maxY - m_minY;
}

float VoxelObject::getWidth()
{
	return m_maxX - m_minX;
}

void VoxelObject::setDepth(float value)
{
	Q_UNUSED(value);
}

void VoxelObject::setHeight(float value)
{
	Q_UNUSED(value);
}

void VoxelObject::setWidth(float value)
{
	Q_UNUSED(value);
}

void VoxelObject::setDrawingMode(DrawingMode mode)
{
	m_drawingMode = mode;
}

void VoxelObject::setColor(QColor color)
{
	m_color = color;
}

const QColor VoxelObject::getColor()
{
	return m_color;
}

void VoxelObject::showBoundingBox(bool val)
{
	m_showBoundingBox = val;
}

bool VoxelObject::getShowBoundingBox()
{
	return m_showBoundingBox;
}

void VoxelObject::setBoundingBoxColor(QColor color)
{
	m_boundingBoxColor = color;
}

QColor VoxelObject::getBoundingBoxColor()
{
	return m_boundingBoxColor;
}

int VoxelObject::addPoint(Point3D point)
{
	m_regCallList = true;
	
	float x = point.getX();
	float y = point.getY();
	float z = point.getZ();
	
	m_minX = MIN(x , m_minX);
	m_minY = MIN(y , m_minY);
	m_minZ = MIN(z , m_minZ);
	
	m_maxX = MAX(x , m_maxX);
	m_maxY = MAX(y , m_maxY);
	m_maxZ = MAX(z , m_maxZ);
	
	Point3D p((int)x, (int)y, (int)z);
	
	int index = -1;
	if ((index = m_pointList.indexOf(p)) == -1)
	{
		int ix = (int)x;
		int iy = (int)y;
		int iz = (int)z;
		
		m_pointList.append(p);
		m_pointMap.insert(ix, QMap<int, QMap<int, bool> >());
		m_pointMap[ix].insert(iy, QMap<int, bool>());
		m_pointMap[ix][iy].insert(iz, true);
		return m_pointList.size();
	}
	return index;
}

void VoxelObject::removePoint(int id)
{
	m_pointList.remove(id);
}

Point3D& VoxelObject::getPoint(int index)
{
	
	return m_pointList[index];
}

Normal& VoxelObject::getNormalAtPoint(int index)
{
	Q_UNUSED(index);
	static Normal n = Normal::null();
	return n;
}

const Normal& VoxelObject::getNormalAtPoint(int index) const
{
	Q_UNUSED(index);
	return Normal::null();
}

int VoxelObject::addFace(const QVector<int>& vertexIndexList)
{
	Q_UNUSED(vertexIndexList);
	return -1;
}

void VoxelObject::removeFace( int id)
{
	Q_UNUSED(id);
}

int VoxelObject::getFaceIndexAtPoint(const Point3D& p) const
{
	Q_UNUSED(p);
	return -1;
}

int VoxelObject::getClosestPointAtPoint(const Point3D &p) const
{
	Q_UNUSED(p);
	return -1;
}

QVector<int> VoxelObject::getPointsInRadius(const Point3D &p, float radius) const
{
	Q_UNUSED(p);
	Q_UNUSED(radius);
	QVector<int> m;
	return m;
}

void VoxelObject::adjustPointNormal(int index)
{
	Q_UNUSED(index);
}

const QVector<Normal>& VoxelObject::getNormalList() const
{
	static QVector<Normal> n;
	
	return n;
}

const PointContainer& VoxelObject::getPointList() const
{
	static Octree<Point> p;
	
	return p;
}

const FaceContainer& VoxelObject::getFaceList() const
{
	static FaceContainer f;
	
	return f;
}

QVector<Normal>& VoxelObject::getNormalList()
{
	static QVector<Normal> n;
	
	return n;
}

PointContainer& VoxelObject::getPointList()
{
	static Octree<Point> p;
	
	return p;
}

FaceContainer& VoxelObject::getFaceList()
{
	static FaceContainer f;
	
	return f;
}

void VoxelObject::lock()
{
	m_mutex.lock();
}

void VoxelObject::unlock()
{
	m_mutex.unlock();
}

void VoxelObject::drawCube(const Point3D& p, uint facesToDraw)
{
	static Point3D dx(1, 0, 0);
	static Point3D dy(0, 1, 0);
	static Point3D dz(0, 0, 1);
	
	Point3D p2 = p + dx;
	Point3D p3 = p + dx + dy;
	Point3D p4 = p + dy;
	
	Point3D p5 = p + dz;
	Point3D p6 = p + dx + dz;
	Point3D p7 = p + dx + dy + dz;
	Point3D p8 = p + dy + dz;
	
	glColor3f(m_color.redF(), m_color.greenF(), m_color.blueF());
	
	if (facesToDraw & Back)
	{
		glVertex3fv(p.getPoint());
		glNormal3f(0, 0, -1);
		glVertex3fv(p2.getPoint());
		glNormal3f(0, 0, -1);
		glVertex3fv(p3.getPoint());
		glNormal3f(0, 0, -1);
		glVertex3fv(p4.getPoint());
		glNormal3f(0, 0, -1);
	}
	
	if (facesToDraw & Front)
	{
		glVertex3fv(p8.getPoint());
		glNormal3f(0, 0, 1);
		glVertex3fv(p7.getPoint());
		glNormal3f(0, 0, 1);
		glVertex3fv(p6.getPoint());
		glNormal3f(0, 0, 1);
		glVertex3fv(p5.getPoint());
		glNormal3f(0, 0, 1);
	}
	
	if (facesToDraw & Right)
	{
		glVertex3fv(p2.getPoint());
		glNormal3f(1, 0, 0);
		glVertex3fv(p6.getPoint());
		glNormal3f(1, 0, 0);
		glVertex3fv(p7.getPoint());
		glNormal3f(1, 0, 0);
		glVertex3fv(p3.getPoint());
		glNormal3f(1, 0, 0);
	}
	
	if (facesToDraw & Left)
	{
		glVertex3fv(p5.getPoint());
		glNormal3f(-1, 0, 0);
		glVertex3fv(p.getPoint());
		glNormal3f(-1, 0, 0);
		glVertex3fv(p4.getPoint());
		glNormal3f(-1, 0, 0);
		glVertex3fv(p8.getPoint());
		glNormal3f(-1, 0, 0);
	}
	
	if (facesToDraw & Bottom)
	{
		glVertex3fv(p.getPoint());
		glNormal3f(0, -1, 0);
		glVertex3fv(p5.getPoint());
		glNormal3f(0, -1, 0);
		glVertex3fv(p6.getPoint());
		glNormal3f(0, -1, 0);
		glVertex3fv(p2.getPoint());
		glNormal3f(0, -1, 0);
	}
	
	if (facesToDraw & Top)
	{
		glVertex3fv(p4.getPoint());
		glNormal3f(0, 1, 0);
		glVertex3fv(p3.getPoint());
		glNormal3f(0, 1, 0);
		glVertex3fv(p7.getPoint());
		glNormal3f(0, 1, 0);
		glVertex3fv(p8.getPoint());
		glNormal3f(0, 1, 0);
	}

}

uint VoxelObject::facesToDraw(const Point3D& p)
{
	int x = (int)p.getX();
	int y = (int)p.getY();
	int z = (int)p.getZ();
	
	uint facesToDraw = ~None;
	if (m_pointMap.contains(x))
		if (m_pointMap.value(x).contains(y))
			if (m_pointMap.value(x).value(y).contains(z+1))
				if (m_pointMap.value(x).value(y).value(z+1))
					facesToDraw = (facesToDraw & ~Front);
	
	if (m_pointMap.contains(x))
		if (m_pointMap.value(x).contains(y))
			if (m_pointMap.value(x).value(y).contains(z-1))
				if (m_pointMap.value(x).value(y).value(z-1))
					facesToDraw = (facesToDraw & ~Back);
	
	if (m_pointMap.contains(x))
		if (m_pointMap.value(x).contains(y+1))
			if (m_pointMap.value(x).value(y+1).contains(z))
				if (m_pointMap.value(x).value(y+1).value(z))
					facesToDraw = (facesToDraw & ~Top);
	
	if (m_pointMap.contains(x))
		if (m_pointMap.value(x).contains(y-1))
			if (m_pointMap.value(x).value(y-1).contains(z))
				if (m_pointMap.value(x).value(y-1).value(z))
					facesToDraw = (facesToDraw & ~Bottom);
	
	if (m_pointMap.contains(x+1))
		if (m_pointMap.value(x+1).contains(y))
			if (m_pointMap.value(x+1).value(y).contains(z))
				if (m_pointMap.value(x+1).value(y).value(z))
					facesToDraw = (facesToDraw & ~Right);
	
	if (m_pointMap.contains(x-1))
		if (m_pointMap.value(x-1).contains(y))
			if (m_pointMap.value(x-1).value(y).contains(z))
				if (m_pointMap.value(x-1).value(y).value(z))
					facesToDraw = (facesToDraw & ~Left);
	return facesToDraw;
}

void VoxelObject::drawBoundingBox()
{
	GLboolean lightEnabled = GL_FALSE;
    lightEnabled = glIsEnabled(GL_LIGHTING);
    
    if (lightEnabled == GL_TRUE)
        glDisable(GL_LIGHTING);
    
	float minX = m_minX - 5;
	float maxX = m_maxX + 5;
	float minY = m_minY - 5;
	float maxY = m_maxY + 5;
	float minZ = m_minZ - 5;
	float maxZ = m_maxZ + 5;
	
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
