/*
 *  Subdivision.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <utility>
#include <vector>

class VertexHandle;
class EdgeHandle;
class SubdivisionScheme;
class Mesh;

/**
 * Class that implement subdivision surfaces.
 */
class DLLEXPORT Subdivision : public ISurface
{
    struct Impl;
    std::unique_ptr<Impl> _d;

  public:
    Subdivision();

    /**
     * Construct a subdivision surface from a @class Mesh.
     */
    Subdivision(const Mesh* mesh);

    virtual ~Subdivision();
    /******************************************************************************
     * IObject3D interface
     */
    virtual size_t iid() const;
    virtual bool   hasChanged();
    virtual void setChanged(bool val);
    virtual void setScene(Scene* scene);
    virtual Scene*                scene() const;
    virtual const IRenderable*    renderable() const;
    virtual const geometry::AABB& boundingBox() const;
    virtual void setColor(const Color& color);
    virtual Color color() const;
    virtual void setSelected(bool val);
    virtual bool                 isSelected() const;
    virtual VertexHandle::size_t addVertex(const Point3& point);
    virtual VertexHandle::size_t addVertex(VertexHandle* v);
    virtual void removeVertex(VertexHandle::size_t id);
    virtual VertexHandle* vertex(VertexHandle::size_t index);
    virtual const VertexHandle* vertex(VertexHandle::size_t index) const;
    virtual VertexHandle::size_t numVertices() const;
    virtual EdgeHandle::size_t addEdge(const EdgeHandle& edge);
    virtual EdgeHandle::size_t addEdge(VertexHandle::size_t v1,
                                       VertexHandle::size_t v2);
    virtual EdgeHandle* edge(EdgeHandle::size_t iid);
    virtual const EdgeHandle* edge(EdgeHandle::size_t iid) const;
    virtual EdgeHandle::size_t numEdges() const;
    virtual FaceHandle::size_t
    addFace(const std::vector<VertexHandle::size_t>& vertexIndexList);
    virtual void
    replaceFace(size_t                                   index,
                const std::vector<VertexHandle::size_t>& vertexIndexList);
    virtual void removeFace(size_t id);
    virtual FaceHandle::size_t numFaces() const;
    virtual FaceHandle* face(FaceHandle::size_t index);
    virtual FaceHandle::size_t getFaceIndexAtPoint(const Point3& p) const;
    virtual VertexHandle::size_t
    getClosestPointAtPoint(const Point3& p) const;
    virtual std::vector<VertexHandle::size_t>
    getPointsInRadius(const Point3& p, float radius) const;
    virtual void adjustPointNormal(VertexHandle::size_t index);
    virtual void lock() const;
    virtual void unlock() const;

    virtual std::vector<VertexHandle::size_t> selectedPoints() const;
    virtual void
    setSelectedPoints(const std::vector<VertexHandle::size_t>& indicesArray);

    virtual Iterator<VertexHandle> vertexIterator();
    virtual Iterator<VertexHandle> constVertexIterator() const;
    virtual Iterator<FaceHandle>   faceIterator();
    virtual Iterator<FaceHandle>   constFaceIterator() const;
    virtual Iterator<EdgeHandle>   edgeIterator();
    virtual Iterator<EdgeHandle>   constEdgeIterator() const;

    // End IObject3D interface

    /**
     * Returns the Edge Iid that has the vertices V1 and V2
     */
    EdgeHandle::size_t edge(VertexHandle::size_t iidVtxTail,
                            VertexHandle::size_t iidVtxHead) const;

    // const Object3D& operator=(const Object3D& obj);

    void addResolutionLevel();
    void removeResolutionLevel(int level);
    void setWorkingResolutionLevel(int level);
    int workingResolutionLevel();

    Iterator<VertexHandle> vertexIterator(int level);
    Iterator<VertexHandle> constVertexIterator(int level) const;
    Iterator<FaceHandle> faceIterator(int level);
    Iterator<FaceHandle> constFaceIterator(int level) const;

    /**
     * Debugging function used to print the memory usage of this
     * object.
     */
    void printMemoryInfo() const;

    /**
     * Convert subdivision surface to a regular Mesh.
     *
     * @param level level of the surface to convert to mesh.
     */
    Mesh* convertToMesh(int level = -1);

    /**
     * Uses mesh as base mesh for this subdivision surface.
     *
     * @param mesh mesh to convert from.
     */
    void convertFromMesh(const Mesh* mesh);

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
