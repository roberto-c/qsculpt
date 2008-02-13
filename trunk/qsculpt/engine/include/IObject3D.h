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
#ifndef IOBJECT3D_H
#define IOBJECT3D_H

#include <QVector>
#include "Point3D.h"
#include "SpEnums.h"
#include "Octree.h"
#include "PointContainer.h"
#include "EdgeContainer.h"
#include "FaceContainer.h"

/*
 * Class forward declaration
 */
class QColor;
class Scene;

typedef QVector<Normal> NormalContainer;

/**
Interface that every 3D object should implement.

  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class IObject3D {
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    IObject3D(){}

    virtual ~IObject3D(){}

    virtual void setScene(Scene* scene) = 0;

    virtual Scene* getScene() const = 0;

    /**
     * Set the object's position.
     *
     * @param x Object's x position
     * @param y Object's y postion
     * @param z Object's z position
     */
    virtual void setPosition (float x, float y, float z) = 0;

    /**
     * Set the object's position. Overloaded method.
     *
     * @param position Object position.
     */
    virtual void setPosition (const Point3D& position) = 0;

    /**
     * Get the object's position.
     *
     * @param x Object x-axis position.
     * @param y Object y-axis position.
     * @param z Object z-axis position.
     */
    virtual void getPosition (float *x, float *y, float *z) const = 0;

    /**
     * Get the object's position. Overloaded method.
     *
     * @return position of the object.
     */
    virtual Point3D getPosition () const = 0;

    /**
     * Move the object.
     * Move the object relative to the actual position.
     *
     * new_position = current_position + delta.
     *
     * @param delta Point that contains the amount of displacement for each axis.
     */
    virtual void displace (const Point3D& delta) = 0;

    /**
     * Rotate the object. This rotation is applied over the local object axis.
     *
     * @param rotX Rotation angle in x-axis.
     * @param rotY Rotation angle in y-axis.
     * @param rotZ Rotation angle in z-axis.
     */
    virtual void rotate (float rotX, float rotY, float rotZ) = 0;

    /**
     * Returns the depth of the object.
     *
     */
    virtual float getDepth() = 0;

    /**
     * Returns the height of the object.
     *
     */
    virtual float getHeight() = 0;

    /**
     * Returns the width of the object.
     *
     */
    virtual float getWidth() = 0;

    /**
     * Sets the depth of the object.
     *
     * @param value new depth value
     */
    virtual void setDepth(float value) = 0;

    /**
     * Sets the height of the object.
     *
     * @param value new height value
     */
    virtual void setHeight(float value) = 0;

    /**
     * Sets the width of the object.
     *
     * @param value new width value
     */
    virtual void setWidth(float value) = 0;

    /**
    * Sets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @param color color of the object.
    *
    */
    virtual void setColor(const QColor& color) = 0;

    /**
    * Gets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @return color of the object.
    *
    */
    virtual QColor getColor() const = 0;

    /**
     * Mark the object as selected. This property is used by the renderers
     * to mark selected objects (for example draw a bounding box around the
     * object)
     *
     * @param val pass true to set the object as selected
     */
    virtual void setSelected(bool val) = 0;

    /**
     * Returns the selected state of the object.
     *
     * @return true if the object is selected, otherwise, false.
     * @see setSelected
     */
    virtual bool isSelected() const = 0;

    /**
     * Set the color of the bounding box.
     *
     * @param color color of bounding box.
     */
    virtual void setBoundingBoxColor(const QColor& color) = 0;

    /**
     * Get the color of the bounding box.
     *
     * @return color of bounding box.
     */
    virtual QColor getBoundingBoxColor() const = 0;

    /**
     * Add point to object. The point added is in local coordinates ( i.e.
     * relative to object's origin.
     *
     * @param point point to add to the object. The point is in local coords.
     *
     * @return ID of the new point.
     */
    virtual int addVertex(const Point3D& point) = 0;

    /**
     * Remove a point from the object. If the point form part of a
     * triangle then that triangle should be removed from the object.
     *
     * @param id ID of the point to delete.
     */
    virtual void removeVertex( int id) = 0;

    /**
     *
     */
    virtual Vertex& getVertex(int index) = 0;

    /**
     *
     */
    virtual Normal& getNormalAtPoint(int index) = 0;

    /**
     *
     */
    virtual const Normal& getNormalAtPoint(int index) const = 0;

	/**
	 *
	 */
	virtual int addEdge(const Edge& edge) = 0;
	
	/**
	 *
	 */
	virtual int addEdge(int v1, int v2) = 0;
	
    /**
     * Add a triangle to the object. The triangle is formed by the vertices
     * passed as arguments. The vertices are passed by means of their ID
     *
     * @param v1 id of the first vertex that form the triangle.
     * @param v2 id of the first vertex that form the triangle.
     * @param v3 id of the first vertex that form the triangle.
     *
     * @return triangle ID
     */
    virtual int addFace( const QVector<int>& vertexIndexes )=0;

	/**
	 * Replace a triangle with new indices to existing points.
	 */
	virtual void replaceFace(int index, const QVector<int>& vertexIndexList)=0;

    /**
     * Remove the triangle from the object. The points or vertices that
     * compound the triangle are not removed from the object. The points
     * could be rendered acording to the drawing mode.
     *
     * @param id triangle ID to remove.
     */
    virtual void removeFace( int id) =0;

    /**
     *
     */
    virtual int getFaceIndexAtPoint(const Point3D& p) const = 0;

    /**
     *
     */
    virtual int getClosestPointAtPoint(const Point3D &p) const = 0;

    /**
     *
     */
    virtual QVector<int> getPointsInRadius(const Point3D &p, float radius) const = 0;

    /**
     *
     */
    virtual void adjustPointNormal(int index) = 0;

    /**
     *
     */
    virtual const NormalContainer& getNormalList() const = 0;

    /**
     *
     */
    virtual const PointContainer& getPointList() const = 0;

    /**
     *
     */
    virtual const FaceContainer& getFaceList() const = 0;

    /**
     *
     */
    virtual NormalContainer& getNormalList() = 0;

    /**
     *
     */
    virtual PointContainer& getPointList()  = 0;

    /**
     *
     */
    virtual FaceContainer& getFaceList()  = 0;

	/**
	 *
	 */
	virtual const EdgeContainer& getEdgeList() const = 0;

	/**
	 *
	 */
	virtual EdgeContainer& getEdgeList() = 0;

    /**
     *
     */
    virtual void lock() const = 0;

    /**
     *
     */
    virtual void unlock() const = 0;

    /**
     *
     */
    virtual void addResolutionLevel() = 0;

    /**
     *
     */
    virtual void removeResolutionLevel(int level) = 0;

    /**
     *
     */
    virtual void setWorkingResolutionLevel(int level) = 0;

    /**
     *
     */
    virtual int getWorkingResolutionLevel() = 0;

    /**
     * Get a buffer with all the vertex data. This data is used to render the
     * mesh using VBOs.
     *
     * The function return a pointer to the buffer data. The pointer should be
     * considerated temporal (do not store the pointer for later use). This is
     * because when inserting new vertices to the mesh, a new allocation may be
     * needed.
     *
     * The function also returns the number of vertices contained in the buffer.
     */
    //virtual bool getVertexBuffer(float **buffer, int* size) = 0;

    /**
     * Get a buffer with all the normal data. This data is meant to be used
     * to create VBOs.
     *
     * The function return a pointer to the buffer data. The pointer should be
     * considerated temporal (do not store the pointer for later use). This is
     * because when inserting new vertices to the mesh, a new allocation may be
     * needed.
     *
     * The function also returns the number of vertices contained in the buffer.
     */
    //virtual bool getNormalBuffer(float **buffer, int* size) = 0;
	
	virtual bool hasChanged()=0;
	
	virtual void setChanged(bool val)=0;
};

#endif
