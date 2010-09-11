/*
 *  untitled.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FACE_H_
#define FACE_H_

#include <QAtomicInt>
#include "Vertex.h"
#include "IIterator.h"

class Edge;
class ISurface;

enum FaceFlags {
    FF_None     = 0,
    FF_Selected = 0x00000001,   /*< Face is selected */
    FF_Deleted  = 0x80000000,  /*< Face is marked as deleted */
    FF_All      = 0xFFFFFFFF
};

/**
 * Face class. This class contains references to points that should
 * form a triangle.
 *
 * The points are indices of the Object3D point list. So, this class It's
 * only meant to be used with the Object3D class (its not a generic triangle
 * class).
 *
 * A triangle its only valid if theirs three points are different. Using the
 * default constructor makes a non valid triangle.
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Face {
    ISurface *_surface; // 4
    Edge *_he; // 4
    Vertex* _vertex; // 4
    Face* _children; // 4

    int _id; // 4
    int _depth; // 4
    FaceFlags _flags; // 4
    
public:
    //qint32 hashValue; // 4

public:
    /**
     * Default constructor. Initiliazes all point to index 0.
     */
    Face(ISurface *surface = NULL);

    /**
     * Construct a new triangle. The triangle is composed by the passed
     * points ids or references.
     *
     * @param p1 first point that form the triangle.
     * @param p2 second point that form the triangle.
     * @param p3 third point that form the triangle.
     */
    Face(ISurface *surface, const QVector<int>& vertexIndexList);

    ~Face();

    /**
     * Get the hash code for this instance
     */
    inline uint hashCode() const;

    /**
     * Gets the instance id of the vertex.
     */
    int iid() const { return _id; }

    /**
     * Set / get attribute flags to the face.
     */
    void addFlag(FaceFlags flag) { _flags = (FaceFlags)(_flags | flag); }
    void removeFlag(FaceFlags flag) { _flags = (FaceFlags)(_flags & ~flag); }
    FaceFlags flags() const { return _flags; }
    void setFlags(FaceFlags flags) {_flags = flags; }

    /**
     * Checks if the triangle data is valid. Triangle is valid only if
     * the three point that compose it are different.
     *
     * @return true if triangle is valid. False, otherwise.
     */
    bool isValid();

    /**
     * Add a vertex to the face. This method allows adding multiple vertices
     * by chaining addVertex calls.
     */
    Face& addEdge(Edge* he);

    /**
     * Adds all the vertices contained in the array.
     */
    void addVertex(const QVector<Vertex*> v);

    /**
     * Gets/sets a reference to a half edge that forms this face.
     */
    Edge* hedge() { return _he; }
    void setHEdge(Edge* hedge);

    /**
     * Sets the first point index reference.
     *
     * @param p1 index of the first point.
     * @param p2 index of the second point.
     * @param p3 index of the third point.
     */
    void setPoints(const QVector<int>& vertexIndexList);

    bool hasEdge(const Edge& /*e*/) const;

    bool hasEdge(int /*v1*/, int /*v2*/) const ;

    bool operator==(const Face& t) const;

    operator Point3() const ;
    operator Point3();

    /**
     * Gets an iterator object to traverse all the vertices that form part
     * of this face.
     */
    Iterator<Vertex> vertexIterator();

    /**
     * Gets an iterator object to traverse all the vertices that form part
     * of this face.
     */
    Iterator<Vertex> constVertexIterator() const;

    /**
     * Gets an iterator object to traverse all the edges that form part
     * of this face.
     */
    Iterator<Edge> edgeIterator();

    /**
     * Gets an iterator object to traverse all the edges that form part
     * of this face.
     */
    Iterator<Edge> constEdgeIterator() const;

protected:
    void subdivide(int level);

    /* Inner classes */
    class VertexIterator;
    class EdgeIterator;

    friend class VertexIterator;
    friend class EdgeIterator;
};

#endif
