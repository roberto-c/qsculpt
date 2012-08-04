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
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Aabb.h>

class Vertex;
class Edge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Mesh : public ISurface
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
    virtual const geometry::AABB& boundingBox() const;
    virtual void setColor(const Color& color);
    virtual Color color() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual size_t addVertex(const Point3& point);
    virtual size_t addVertex(Vertex* v);
    virtual void removeVertex(size_t id);
    virtual Vertex* vertex(size_t index);
    virtual const Vertex* vertex(size_t index) const;
    virtual size_t numVertices() const;
    virtual size_t addEdge(const Edge& edge);
    virtual size_t addEdge(size_t v1, size_t v2);
    virtual size_t addFace(const std::vector<size_t>& vertexIndexList);
    virtual void replaceFace(size_t index, const std::vector<size_t>& vertexIndexList);
    virtual void removeFace( size_t id);
    virtual size_t numFaces() const;
    virtual Face* face(size_t index);
    virtual size_t getFaceIndexAtPoint(const Point3& p) const;
    virtual size_t getClosestPointAtPoint(const Point3 &p) const;
    virtual std::vector<size_t> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(size_t index);
    virtual void lock() const;
    virtual void unlock() const;
    
    virtual std::vector<size_t> selectedPoints() const;
    virtual void setSelectedPoints(const std::vector<size_t>& indicesArray);
    
    virtual Iterator<Vertex> vertexIterator();
    virtual Iterator<Vertex> constVertexIterator() const;
    virtual Iterator<Face> faceIterator();
    virtual Iterator<Face> constFaceIterator() const;
    virtual Iterator<Edge> edgeIterator();
    virtual Iterator<Edge> constEdgeIterator() const;
    
    
    // End IObject3D interface
    
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
    size_t addEdge(Vertex* v1, Vertex* v2);
    
    
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

