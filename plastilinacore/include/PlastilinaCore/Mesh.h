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

class VertexHandle;
class EdgeHandle;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class DLLEXPORT Mesh : public ISurface, public IRenderable
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
    virtual VertexHandle::size_t addVertex(const Point3& point);
    virtual VertexHandle::size_t addVertex(VertexHandle* v);
    virtual void removeVertex(VertexHandle::size_t id);
    virtual VertexHandle* vertex(VertexHandle::size_t index);
    virtual const VertexHandle* vertex(VertexHandle::size_t index) const;
    virtual VertexHandle::size_t numVertices() const;
    virtual EdgeHandle::size_t addEdge(const EdgeHandle& edge);
    virtual EdgeHandle::size_t addEdge(VertexHandle::size_t v1, VertexHandle::size_t v2);
	virtual EdgeHandle::size_t numEdges() const;
    virtual FaceHandle::size_t addFace(const std::vector<VertexHandle::size_t>& vertexIndexList);
    virtual void replaceFace(FaceHandle::size_t index, const std::vector<VertexHandle::size_t>& vertexIndexList);
    virtual void removeFace( FaceHandle::size_t id);
    virtual FaceHandle::size_t numFaces() const;
    virtual FaceHandle* face(FaceHandle::size_t index);
    virtual FaceHandle::size_t getFaceIndexAtPoint(const Point3& p) const;
    virtual VertexHandle::size_t getClosestPointAtPoint(const Point3 &p) const;
    virtual std::vector<VertexHandle::size_t> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(VertexHandle::size_t index);
    virtual void lock() const;
    virtual void unlock() const;
    
    virtual std::vector<VertexHandle::size_t> selectedPoints() const;
    virtual void setSelectedPoints(const std::vector<VertexHandle::size_t>& indicesArray);
    
    virtual Iterator<VertexHandle> vertexIterator();
    virtual Iterator<VertexHandle> constVertexIterator() const;
    virtual Iterator<FaceHandle> faceIterator();
    virtual Iterator<FaceHandle> constFaceIterator() const;
    virtual Iterator<EdgeHandle> edgeIterator();
    virtual Iterator<EdgeHandle> constEdgeIterator() const;
    
    
    // End IObject3D interface
	
	// IRenderable
	virtual void 						render(RenderState & state) const;
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
	EdgeHandle::size_t addEdge(VertexHandle* v1, VertexHandle* v2);
    
    
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

