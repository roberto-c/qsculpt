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
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <utility>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Aabb.h>

class Vertex;
class Edge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Mesh : public ISurface, public IRenderable
{
    struct Impl;
    std::unique_ptr<Impl> _d;
    
public:
    Mesh();
    virtual ~Mesh();
    /******************************************************************************
     * IObject3D interface
     */
    virtual size_t iid() const;
    virtual bool hasChanged();
    virtual void setChanged(bool val);
    virtual void setScene(Scene* scene) ;
    virtual Scene* scene() const;
	virtual const IRenderable* renderable() const;
    virtual const geometry::AABB& boundingBox() const;
    virtual void setColor(const Color& color);
    virtual Color color() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual Vertex::size_t addVertex(const Point3& point);
    virtual Vertex::size_t addVertex(Vertex* v);
    virtual void removeVertex(Vertex::size_t id);
    virtual Vertex* vertex(Vertex::size_t index);
    virtual const Vertex* vertex(Vertex::size_t index) const;
    virtual Vertex::size_t numVertices() const;
    virtual Edge::size_t addEdge(const Edge& edge);
    virtual Edge::size_t addEdge(Vertex::size_t v1, Vertex::size_t v2);
	virtual Edge::size_t numEdges() const;
    virtual Face::size_t addFace(const std::vector<Vertex::size_t>& vertexIndexList);
    virtual void replaceFace(Face::size_t index, const std::vector<Vertex::size_t>& vertexIndexList);
    virtual void removeFace( Face::size_t id);
    virtual Face::size_t numFaces() const;
    virtual Face* face(Face::size_t index);
    virtual Face::size_t getFaceIndexAtPoint(const Point3& p) const;
    virtual Vertex::size_t getClosestPointAtPoint(const Point3 &p) const;
    virtual std::vector<Vertex::size_t> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(Vertex::size_t index);
    virtual void lock() const;
    virtual void unlock() const;
    
    virtual std::vector<Vertex::size_t> selectedPoints() const;
    virtual void setSelectedPoints(const std::vector<Vertex::size_t>& indicesArray);
    
    virtual Iterator<Vertex> vertexIterator();
    virtual Iterator<Vertex> constVertexIterator() const;
    virtual Iterator<Face> faceIterator();
    virtual Iterator<Face> constFaceIterator() const;
    virtual Iterator<Edge> edgeIterator();
    virtual Iterator<Edge> constEdgeIterator() const;
    
    
    // End IObject3D interface
	
	// IRenderable
	virtual void 						render(const RenderState * state) const;
    // End IRendeable
    
    //const Object3D& operator=(const Object3D& obj);
    
    /**
     * Debugging function used to print the memory usage of this
     * object.
     */
    void printMemoryInfo() const;
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();
    
    /**
     *
     */
	Edge::size_t addEdge(Vertex* v1, Vertex* v2);
    
    
    // inner classes
protected:
    class VertexIterator;
    class FaceIterator;
    class EdgeIterator;
    
    friend class VertexIterator;
    friend class FaceIterator;
    friend class EdgeIterator;
};



#endif

