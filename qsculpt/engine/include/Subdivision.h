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
#include "Aabb.h"

class Vertex;
class Edge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Subdivision : public ISurface
{
    struct Impl;
    QScopedPointer<Impl> _d;
    
//    typedef std::pair<int, int>     VtxPair;
//    typedef QHash<int, Vertex*>     VertexCollection;
//    typedef QHash<int, Edge*>       EdgesCollection;
//    typedef QHash<VtxPair, int>     VtxPairEdgeMap;
//    typedef QHash<int, Face*>       FacesCollection;
//
//    /** Instance ID of the surface */
//    uint _iid;
//    
//    /** Vertices by each subdivision level */
//    std::vector<VertexCollection*>  _vertLevelCollections;
//    /** Edges by each subdivision level */
//    std::vector<EdgesCollection*>   _edgesLevelCollections;
//    /** Relates a pair of vertex ids to an edge*/
//    std::vector<VtxPairEdgeMap*>    _vertexEdgeCollection;
//
//    std::vector<FacesCollection*>   _facesLevelCollections;
//
//
//    VertexCollection    *_vertices;
//    EdgesCollection     *_edges;
//    VtxPairEdgeMap      *_vtxPairEdge;
//    FacesCollection     *_faces;
//
//    SubdivisionScheme* scheme;

public:
    Subdivision();
    virtual ~Subdivision();
    /******************************************************************************
     * IObject3D interface
     */
    virtual uint iid() const;
    virtual bool hasChanged();
    virtual void setChanged(bool val);
    virtual void setScene(Scene* scene) ;
    virtual Scene* getScene() const;
    virtual Point3 getPosition() const;
    virtual void displace(const Point3& delta);
    virtual void getPosition(float *x, float *y, float *z) const;
    virtual void setOrientation(float rotX, float rotY, float rotZ);
    virtual void orientation(float& rotX, float& rotY, float& rotZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(const Point3& position);
    virtual const geometry::AABB& getBoundingBox() const;
    virtual void setColor(const QColor& color);
    virtual QColor getColor() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual int addVertex(const Point3& point);
    virtual int addVertex(Vertex* v);
    virtual void removeVertex(int id);
    virtual Vertex* getVertex(int index);
    virtual const Vertex* getVertex(int index) const;
    virtual int getNumVertices() const;
    virtual int addEdge(const Edge& edge);
    virtual int addEdge(int v1, int v2);
    virtual int addFace(const QVector<int>& vertexIndexList);
    virtual void replaceFace(int index, const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
    virtual int getNumFaces() const;
    virtual Face* getFace(int index);
    virtual int getFaceIndexAtPoint(const Point3& p) const;
    virtual int getClosestPointAtPoint(const Point3 &p) const;
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual void lock() const;
    virtual void unlock() const;
    virtual void addResolutionLevel();
    virtual void removeResolutionLevel(int level);
    virtual void setWorkingResolutionLevel(int level);
    virtual int getWorkingResolutionLevel();

    virtual QVector<int> getSelectedPoints() const;
    virtual void setSelectedPoints(const QVector<int>& indicesArray);

    virtual Iterator<Vertex> vertexIterator();
    virtual Iterator<Vertex> constVertexIterator() const;
    virtual Iterator<Face> faceIterator();
    virtual Iterator<Face> constFaceIterator() const;

    Point3 localToWorldCoords(const Point3& p) const;

    Point3 worldToLocalCoords(const Point3& p) const;

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
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();

    /**
     * Draw a bounding box around the object.
     */
    //void drawBoundingBox();

    /**
      *
      */
    int addEdge(Vertex* v1, Vertex* v2);


    // inner classes
protected:
    class VertexIterator;
    class FaceIterator;

    friend class VertexIterator;
    friend class FaceIterator;
};

#endif
