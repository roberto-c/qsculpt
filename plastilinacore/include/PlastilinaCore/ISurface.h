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

#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/SpEnums.h>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/Vertex.h>
#include <PlastilinaCore/Face.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/HEdge.h>


/*
 * Class forward declaration
 */
class Color;
class Scene;
class IRenderer;
class IRenderable;

/**
Interface that every 3D object should implement.

  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class ISurface {
    
public:
	typedef uint32_t size_t;
    typedef std::shared_ptr<ISurface>   shared_ptr;
    typedef std::weak_ptr<ISurface>     weak_ptr;
    typedef std::unique_ptr<ISurface>   Ptr;
    
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
    virtual size_t iid() const = 0;
    
    
    virtual void setScene(Scene* scene) = 0;

    virtual Scene* scene() const = 0;
	
	/**
	 * Returns the renderer to use to display this surface.
	 */
	virtual const IRenderable* renderable() const = 0;
	
    /**
     * Returns the bounding box of the object.
     *
     */
    virtual const geometry::AABB& boundingBox() const = 0;
	
	/**
	 *
	 */
	// virtual const geometry::Sphere& boundingSphere() const = 0;

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
    virtual Vertex::size_t addVertex(const Point3& point) = 0;

    /**
     * Add vertex v to this object. The object will mantain the reference
     * to the vertex. Do not delete the vertex while the object has a reference
     * to it.
     *
     * @param v vertex to add to the object.
     *
     * @return the id of the vertex inside the object.
     */
    virtual Vertex::size_t addVertex(Vertex* v) = 0;

    /**
     * Remove a point from the object. If the point form part of a
     * triangle then that triangle should be removed from the object.
     *
     * @param id ID of the point to delete.
     */
    virtual void removeVertex( Vertex::size_t id) = 0;

    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual Vertex* vertex(Vertex::size_t iid) = 0;
    
    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual const Vertex* vertex(Vertex::size_t iid) const = 0;

    /**
     * Gets the number of vertices
     */
    virtual Vertex::size_t numVertices() const = 0;

    /**
     *
     */
    virtual Edge::size_t addEdge(const Edge& edge) = 0;

    /**
     *
     */
    virtual Edge::size_t addEdge(Vertex::size_t v1, Vertex::size_t v2) = 0;
	
	/**
	 *
	 */
	virtual Edge::size_t numEdges() const = 0;

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
    virtual Face::size_t addFace( const std::vector<Vertex::size_t>& vertexIndexes )=0;

    /**
     * Replace a triangle with new indices to existing points.
     */
    virtual void replaceFace(Face::size_t index,
							 const std::vector<Vertex::size_t>& vertexIndexList)=0;

    /**
     * Remove the triangle from the object. The points or vertices that
     * compound the triangle are not removed from the object. The points
     * could be rendered acording to the drawing mode.
     *
     * @param id triangle ID to remove.
     */
    virtual void removeFace( Face::size_t id) =0;

    /**
     * Returns the number of faces that the object has.
     *
     * TODO: This may not belong here as is specific to mesh surfaces.
     */
    virtual Face::size_t numFaces() const = 0;

    /**
     * Returns the face at the position index.
     */
    virtual Face* face(Face::size_t index) = 0;

    /**
     *
     */
    virtual Face::size_t getFaceIndexAtPoint(const Point3& p) const = 0;

    /**
     *
     */
    virtual Vertex::size_t getClosestPointAtPoint(const Point3 &p) const = 0;

    /**
     *
     */
    virtual std::vector<Vertex::size_t> getPointsInRadius(const Point3 &p, float radius) const = 0;

    /**
     *
     */
    virtual void adjustPointNormal(size_t index) = 0;

    /**
     *
     */
    virtual void lock() const = 0;

    /**
     *
     */
    virtual void unlock() const = 0;

    /**
     * Flag to check if the surface has been modified.
     */
    virtual bool hasChanged()=0;

    /**
     * Sets a flag to mark the surface as modified.
     */
    virtual void setChanged(bool val)=0;

    /**
     *
     */
    virtual std::vector<Vertex::size_t> selectedPoints() const = 0;
    
    /*
     *
     */
    virtual void setSelectedPoints(const std::vector<Vertex::size_t>& indicesArray) = 0;

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
     * Used to notify mesh has changed.
     */
    void meshChanged(ISurface* mesh) {
        
    }
};

typedef std::vector<ISurface*> ObjectContainer;

#endif
