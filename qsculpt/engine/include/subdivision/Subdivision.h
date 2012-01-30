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
#include "ISurface.h"
#include "SceneNode.h"
#include <QHash>
#include "geometry/Aabb.h"

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
    QScopedPointer<Impl> _d;

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
    virtual uint iid() const;
    virtual bool hasChanged();
    virtual void setChanged(bool val);
    virtual void setScene(Scene* scene) ;
    virtual Scene* scene() const;
    virtual const geometry::AABB& boundingBox() const;
    virtual void setColor(const Color& color);
    virtual Color color() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual int addVertex(const Point3& point);
    virtual int addVertex(Vertex* v);
    virtual void removeVertex(int id);
    virtual Vertex* vertex(int index);
    virtual const Vertex* vertex(int index) const;
    virtual int numVertices() const;
    virtual int addEdge(const Edge& edge);
    virtual int addEdge(int v1, int v2);
    virtual int addFace(const QVector<int>& vertexIndexList);
    virtual void replaceFace(int index, const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
    virtual int numFaces() const;
    virtual Face* face(int index);
    virtual int getFaceIndexAtPoint(const Point3& p) const;
    virtual int getClosestPointAtPoint(const Point3 &p) const;
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual void lock() const;
    virtual void unlock() const;
    
    
    void addResolutionLevel();
    void removeResolutionLevel(int level);
    void setWorkingResolutionLevel(int level);
    int workingResolutionLevel();

    virtual QVector<int> selectedPoints() const;
    virtual void setSelectedPoints(const QVector<int>& indicesArray);

    virtual Iterator<Vertex> vertexIterator();
    virtual Iterator<Vertex> constVertexIterator() const;
    virtual Iterator<Face> faceIterator();
    virtual Iterator<Face> constFaceIterator() const;
    virtual Iterator<Edge> edgeIterator();
    virtual Iterator<Edge> constEdgeIterator() const;

    // End IObject3D interface

    //const Object3D& operator=(const Object3D& obj);

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
    int addEdge(Vertex* v1, Vertex* v2);


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
