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
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <QColor>
#include <QMutex>
#include "iobject3d.h"
#include "point3d.h"


class Scene;
struct Face;
struct Edge;
struct Point;

/**
 * Base class for all 3D objects.
 *
 *  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Object3D : public IObject3D
{
public:
    Object3D();

    Object3D(const Object3D& cpy);
    
    virtual ~Object3D();
    
/******************************************************************************
 * IObject3D interface
 */
    virtual void setScene(Scene* scene) ;
    virtual Scene* getScene() const;
    virtual Point3D getPosition() const;
    virtual void displace(const Point3D& delta);
    virtual void getPosition(float *x, float *y, float *z) const;
    virtual void rotate(float rotX, float rotY, float rotZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(const Point3D& position);
    virtual void draw();
    virtual float getDepth();
    virtual float getHeight();
    virtual float getWidth();
    virtual void setDepth(float value);
    virtual void setHeight(float value);
    virtual void setWidth(float value);
    virtual void setDrawingMode(DrawingMode mode);
    virtual void setColor(const QColor& color);
    virtual const QColor getColor();
    virtual void showBoundingBox(bool val);
    virtual bool getShowBoundingBox();
    virtual void setBoundingBoxColor(const QColor& color);
    virtual QColor getBoundingBoxColor();
    virtual int addVertex(const Vertex& point);
    virtual void removeVertex(int id);
    virtual Vertex& getVertex(int index);
    virtual Normal& getNormalAtPoint(int index);
    virtual const Normal& getNormalAtPoint(int index) const;
    virtual int addFace(const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
    virtual int getFaceIndexAtPoint(const Point3D& p) const;
    virtual int getClosestPointAtPoint(const Point3D &p) const;
    virtual QVector<int> getPointsInRadius(const Point3D &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual const QVector<Normal>& getNormalList() const;
    virtual const PointContainer& getPointList() const;
    virtual const FaceContainer& getFaceList() const;
    virtual QVector<Normal>& getNormalList();
    virtual PointContainer& getPointList();
    virtual FaceContainer& getFaceList();
    virtual void lock();
    virtual void unlock();
// End IObject3D interface

    const Object3D& operator=(const Object3D& obj);
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();
    
    /**
     * Draw the object with lines.
     */
    virtual void drawWire();
    
    /**
     * Draw the object with smooth rendering.
     */
    virtual void drawSmooth();
    
    /**
     * Draw the object as points.
     */
    virtual void drawPoints();
    
    /**
     * Draw the object with flat surfaces.
     */
    virtual void drawFlat();
    
    /**
     * Draw a bounding box around the object.
     */
    void drawBoundingBox();
    
    /**
     * Scales the object to a factor. This method loops by each point and
     * applies the corresponding factor of each component.
     * 
     * @param xfactor factor to apply on the x compoent of each point.
     * @param yfactor factor to apply on the y compoent of each point.
     * @param zfactor factor to apply on the z compoent of each point.
     */
    void scale(float xfactor, float yfactor, float zfactor);
    
    /**
     * 
     */
    Point3D computeFaceNormal(int index);
    
    /**
     * 
     */
    Point3D computeFaceNormal(Face &face);
    
    /**
     * 
     */
    void computeAllNormals();
    
    void drawVertexNormals();
    
    Scene*          m_scene;
    Point3D         m_position;
    DrawingMode     m_drawingMode;
    QColor          m_color,
                    m_boundingBoxColor;
    Point3D         m_boundingBoxVert[8];
    float           m_minX,
                    m_maxX,
                    m_minY,
                    m_maxY,
                    m_minZ,
                    m_maxZ;
    float           m_rotX, m_rotY, m_rotZ;
    bool            m_showBoundingBox;
    int             m_callListId;
    bool            m_genereateCallList;
    
private:
    void updateBoundingBox();
    
    PointContainer  m_pointList;
    QVector<Normal> m_normalList;
    FaceContainer   m_faceList;
    QMutex          m_mutex;
};

#endif

