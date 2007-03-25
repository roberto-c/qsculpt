/*
 *  VoxelObject.h
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 2/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VOXELOBJECT_H
#define VOXELOBJECT_H

#include <qcolor.h>
#include <QVector>
#include <QMap>
#include <QMutex>
#include "iobject3d.h"
#include "point3d.h"
#include "octree.h"

class Scene;
struct Face;
struct Edge;
struct Point;

/**
Base class for all 3D objects.
 
	@author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class VoxelObject : public IObject3D
{
public:
    VoxelObject();
	
    VoxelObject(const VoxelObject& cpy);
    
    virtual ~VoxelObject();
    
	/******************************************************************************
		* IObject3D interface
		*/
    virtual void setScene(Scene* scene) ;
    virtual Scene* getScene() const;
    virtual Point3D getPosition() const;
    virtual void displace(Point3D delta);
    virtual void getPosition(float *x, float *y, float *z) const;
    virtual void rotate(float rotX, float rotY, float rotZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(Point3D position);
    virtual void draw();
    virtual float getDepth();
    virtual float getHeight();
    virtual float getWidth();
    virtual void setDepth(float value);
    virtual void setHeight(float value);
    virtual void setWidth(float value);
    virtual void setDrawingMode(DrawingMode mode);
    virtual void setColor(QColor color);
    virtual const QColor getColor();
    virtual void showBoundingBox(bool val);
    virtual bool getShowBoundingBox();
    virtual void setBoundingBoxColor(QColor color);
    virtual QColor getBoundingBoxColor();
    virtual int addPoint(Point3D point);
    virtual void removePoint(int id);
    virtual Point3D& getPoint(int index);
    virtual Normal& getNormalAtPoint(int index);
    virtual const Normal& getNormalAtPoint(int index) const;
    virtual int addFace(const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
    //virtual void subdivide();
	virtual int getFaceIndexAtPoint(const Point3D& p) const;
	virtual int getClosestPointAtPoint(const Point3D &p) const;
	virtual QVector<int> getPointsInRadius(const Point3D &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual const QVector<Normal>& getNormalList() const;
    virtual const Octree<Point>& getPointList() const;
    virtual const QVector<Face>& getFaceList() const;
	virtual QVector<Normal>& getNormalList();
    virtual Octree<Point>& getPointList();
    virtual QVector<Face>& getFaceList();
	virtual void lock();
	virtual void unlock();
	// End IObject3D interface
	
private:
		enum CubeFaces {
			None = 0,
			Front = 0x01,
			Back = 0x02,
			Right = 0x04,
			Left = 0x08,
			Top = 0x10,
			Bottom = 0x20
		};
	
	void		drawCube(const Point3D& p, uint facesToDraw);
	uint	facesToDraw(const Point3D& p);
	
	/**
		* Draw a bounding box around the object.
     */
    void drawBoundingBox();
	
protected:
	
		Scene*		m_scene;
		Point3D		m_position;
		DrawingMode m_drawingMode;
		QColor		m_color,
					m_boundingBoxColor;
		Point3D		m_boundingBoxVert[8];
		float		m_minX,
					m_maxX,
					m_minY,
					m_maxY,
					m_minZ,
					m_maxZ;
		float		m_rotX,
					m_rotY,
					m_rotZ;
		bool		m_showBoundingBox;
		int			m_callListId;
		bool		m_regCallList;
		
private:
		QVector<Point3D>	m_pointList;
		QMap<int, QMap<int, QMap<int, bool> > > m_pointMap;
		QMutex			m_mutex;
};

#endif
