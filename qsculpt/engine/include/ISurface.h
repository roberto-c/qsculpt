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

#include <QObject>
#include <QVector>
#include "Point3D.h"
#include "SpEnums.h"
#include "IIterator.h"
#include "Vertex.h"
#include "Face.h"
#include "geometry/Aabb.h"


/*
 * Class forward declaration
 */
class Color;
class Scene;

/**
Interface that every 3D object should implement.

  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class ISurface : public QObject {
    Q_OBJECT
    Q_PROPERTY(Point3 position READ position WRITE setPosition)
    
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    ISurface(){}

    virtual ~ISurface(){}

    /**
     * Instance id of the surface
     */
    virtual uint iid() const = 0;
    
    
    virtual void setScene(Scene* scene) = 0;

    virtual Scene* scene() const = 0;

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
    virtual void setPosition (const Point3& position) = 0;

    /**
     * Get the object's position.
     *
     * @param x Object x-axis position.
     * @param y Object y-axis position.
     * @param z Object z-axis position.
     */
    virtual void position (float *x, float *y, float *z) const = 0;

    /**
     * Get the object's position. Overloaded method.
     *
     * @return position of the object.
     */
    virtual Point3 position () const = 0;

    /**
     * Move the object.
     * Move the object relative to the actual position.
     *
     * new_position = current_position + delta.
     *
     * @param delta Point that contains the amount of displacement for each axis.
     */
    virtual void displace (const Point3& delta) = 0;

    /**
     * Rotate the object. This rotation is applied over the local object axis.
     *
     * @param rotX Rotation angle in x-axis.
     * @param rotY Rotation angle in y-axis.
     * @param rotZ Rotation angle in z-axis.
     */
    virtual void setOrientation(float rotX, float rotY, float rotZ) = 0;

    /**
     * Returns the orientation of the object in Euler angle
     */
    virtual void orientation(float& rotX, float& rotY, float& rotZ) = 0;
    
    
    /**
     *
     */
    virtual Eigen::Affine3f transform() const = 0;
    
    /**
     *
     */
    virtual Eigen::Affine3f & transform() = 0;
    
    /**
     *
     */
    virtual void setTransform(const Eigen::Affine3f & transform) = 0;
    
    /**
     * Returns the bounding box of the object.
     *
     */
    virtual const geometry::AABB& boundingBox() const = 0;

    /**
    * Sets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @param color color of the object.
    *
    */
    virtual void setColor(const Color& color) = 0;

    /**
    * Gets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @return color of the object.
    *
    */
    virtual Color color() const = 0;

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
     * Add point to object. The point added is in local coordinates ( i.e.
     * relative to object's origin.
     *
     * @param point point to add to the object. The point is in local coords.
     *
     * @return ID of the new point.
     */
    virtual int addVertex(const Point3& point) = 0;

    /**
     * Add vertex v to this object. The object will mantain the reference
     * to the vertex. Do not delete the vertex while the object has a reference
     * to it.
     *
     * @param v vertex to add to the object.
     *
     * @return the id of the vertex inside the object.
     */
    virtual int addVertex(Vertex* v) = 0;

    /**
     * Remove a point from the object. If the point form part of a
     * triangle then that triangle should be removed from the object.
     *
     * @param id ID of the point to delete.
     */
    virtual void removeVertex( int id) = 0;

    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual Vertex* vertex(int iid) = 0;
    
    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual const Vertex* vertex(int iid) const = 0;

    /**
     * Gets the number of vertices
     */
    virtual int numVertices() const = 0;

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
     * Returns the number of faces that the object has.
     *
     * TODO: This may not belong here as is specific to mesh surfaces.
     */
    virtual int numFaces() const = 0;

    /**
     * Returns the face at the position index.
     */
    virtual Face* face(int index) = 0;

    /**
     *
     */
    virtual int getFaceIndexAtPoint(const Point3& p) const = 0;

    /**
     *
     */
    virtual int getClosestPointAtPoint(const Point3 &p) const = 0;

    /**
     *
     */
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const = 0;

    /**
     *
     */
    virtual void adjustPointNormal(int index) = 0;

    /**
     *
     */
    virtual void lock() const = 0;

    /**
     *
     */
    virtual void unlock() const = 0;

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

    /**
     *
     */
    virtual QVector<int> selectedPoints() const = 0;
    
    /*
     *
     */
    virtual void setSelectedPoints(const QVector<int>& indicesArray) = 0;

    /**
         * Returns a vertex iterator to traverse over all the vertices in this
         * object.
         */
    virtual Iterator<Vertex> vertexIterator() = 0;
    virtual Iterator<Vertex> constVertexIterator() const = 0;

    /**
     * Returns a face iterator to traverse over all the faces of this object.
     */
    virtual Iterator<Face> faceIterator() = 0;

    /**
     * Return a const iterator over the faces that contained in this object.
     */
    virtual Iterator<Face> constFaceIterator() const = 0;

    /**
     *
     */
    virtual Iterator<Edge> edgeIterator() = 0;
    
    /**
     *
     */
    virtual Iterator<Edge> constEdgeIterator() const = 0;

    /**
     * This function applies a transformation to convert a coordinate in
     * the local coordinate system to the world coordinate system.
     *
     * @param p point to convert to world coordinate system.
     *
     * @return a point in the world coornidate system.
     */
    virtual Point3 localToWorldCoords(const Point3& p) const = 0;

    /**
     * This function applies a transformation to convert a coordinate in
     * the world coordinate system to the local coordinate system.
     *
     * @param p point to convert to local coordinate system.
     *
     * @return a point in the local coordinate system.
     */
    virtual Point3 worldToLocalCoords(const Point3& p) const = 0;

signals:
    void meshChanged(ISurface* mesh);

    void positionChanged(float x, float y, float z);
};

typedef QVector<ISurface*> ObjectContainer;

#endif
