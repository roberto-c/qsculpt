//
//  PointCloud.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//
//
#pragma once
#ifndef PlastilinaCore_PointCloud_h
#define PlastilinaCore_PointCloud_h

#include <PlastilinaCore/ISurface.h>

class PointCloud : public ISurface
{
    
public:
    typedef std::shared_ptr<PointCloud>   shared_ptr;
    typedef std::weak_ptr<PointCloud>     weak_ptr;
    typedef std::unique_ptr<PointCloud>   Ptr;
    
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    PointCloud();
    
    virtual ~PointCloud();
    
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
    
    /**
     * Used to notify mesh has changed.
     */
    void meshChanged(ISurface* mesh);
    
private:
    struct Impl;
    std::unique_ptr<Impl>	d;
    
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
