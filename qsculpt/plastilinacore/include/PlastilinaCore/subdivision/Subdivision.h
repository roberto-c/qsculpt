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

#include <vector>
#include <utility>
#include <CoreEngine/ISurface.h>
#include <CoreEngine/SceneNode.h>
#include <CoreEngine/geometry/Aabb.h>

class Vertex;
class Edge;
class SubdivisionScheme;
class Mesh;

/**
 * Class that implement subdivision surfaces.
 */
class Subdivision : public ISurface
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
    virtual size_t              iid() const;
    virtual bool                hasChanged();
    virtual void                setChanged(bool val);
    virtual void                setScene(Scene* scene) ;
    virtual Scene*              scene() const;
    virtual const               geometry::AABB& boundingBox() const;
    virtual void                setColor(const Color& color);
    virtual Color               color() const;
    virtual void                setSelected(bool val);
    virtual bool                isSelected() const;
    virtual size_t              addVertex(const Point3& point);
    virtual size_t              addVertex(Vertex* v);
    virtual void                removeVertex(size_t id);
    virtual Vertex*             vertex(size_t index);
    virtual const               Vertex* vertex(size_t index) const;
    virtual size_t              numVertices() const;
    virtual size_t              addEdge(const Edge& edge);
    virtual size_t              addEdge(size_t v1, size_t v2);
    virtual size_t              addFace(const std::vector<size_t>& vertexIndexList);
    virtual void                replaceFace(size_t index, 
                                            const std::vector<size_t>& vertexIndexList);
    virtual void                removeFace( size_t id);
    virtual size_t              numFaces() const;
    virtual Face*               face(size_t index);
    virtual size_t              getFaceIndexAtPoint(const Point3& p) const;
    virtual size_t              getClosestPointAtPoint(const Point3 &p) const;
    virtual std::vector<size_t>     getPointsInRadius(const Point3 &p, 
                                                  float radius) const;
    virtual void                adjustPointNormal(size_t index);
    virtual void                lock() const;
    virtual void                unlock() const;

    virtual std::vector<size_t>     selectedPoints() const;
    virtual void                setSelectedPoints(const std::vector<size_t>& indicesArray);

    virtual Iterator<Vertex>    vertexIterator();
    virtual Iterator<Vertex>    constVertexIterator() const;
    virtual Iterator<Face>      faceIterator();
    virtual Iterator<Face>      constFaceIterator() const;
    virtual Iterator<Edge>      edgeIterator();
    virtual Iterator<Edge>      constEdgeIterator() const;

    // End IObject3D interface
    
    /**
     * Returns the Edge Iid that has the vertices V1 and V2
     */
    size_t                      edge(size_t iidVtxTail, size_t iidVtxHead) const;

    //const Object3D& operator=(const Object3D& obj);

    void            addResolutionLevel();
    void            removeResolutionLevel(int level);
    void            setWorkingResolutionLevel(int level);
    int             workingResolutionLevel();
    
    Iterator<Vertex> vertexIterator(int level);
    Iterator<Vertex> constVertexIterator(int level) const;
    Iterator<Face> faceIterator(int level);
    Iterator<Face> constFaceIterator(int level) const;

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
