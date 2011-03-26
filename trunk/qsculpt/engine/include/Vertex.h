/*
 *  Vertex.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VERTEX_H
#define VERTEX_H

#include <Eigen/Core>
#include <QAtomicInt>
#include "Point3D.h"
#include "IIterator.h"

class Edge;

enum VertexFlags {
    VF_None     = 0,
    VF_Selected = 0x00000001, /*< Vertex is selected*/
    VF_Deleted  = 0x80000000, /*< Vertex is marked as deleted.*/
    VF_ALL      = 0xFFFFFFFF
};

class Vertex
{
    static QAtomicInt NEXT_ID;

    Point3      _position; // 16
    Vector3     _normal; // 16
    Vector3     _color; // 16
    int         _id; //4
    VertexFlags _flags; // 4
    Edge*       _he; // 4

    class VertexIterator;

public:

    /**
     * Contructs a new vertex instance
     */
    Vertex();

    /**
     * Construct a new vertex with the position specified
     */
    Vertex(const Point3 & position,
           const Vector3 & normal = Vector3(0, 1, 0),
           const Vector3 & color = Vector3(1, 1, 1)
                                   );

    /**
     * Copy constructor. Creates a new instance of a vertex
     * with the same attributes as another vertex.
     *
     * The new vertex have the same half edge assigned as the original
     * vertex.
     */
    Vertex(const Vertex&);

    /**
     * Gets the instance id of the vertex.
     */
    int iid() const { return _id; }

    /**
     * Gets / sets the position of the vertex
     */
    Point3 getPosition() const { return _position; }
    Point3 position() const { return _position; }
    Point3& position() { return _position; }

    /**
     * Get / sets the color attribute of the vertex;
     */
    Vector3 color() const { return _color; }
    Vector3& color() { return _color; }

    /**
     * Gets / sets the normal attribute for this vertex instance
     */
    Vector3 normal() const { return _normal; }
    Vector3& normal() { return _normal; }

    /**
     * Assign or returns one of the half edges associated to the vertex.
     */
    void setEdge(Edge* he) { _he = he; }
    Edge* edge() const { return _he; }

    /**
     * Set / get attribute flags to the vertex.
     */
    void addFlag(VertexFlags flag) { _flags = (VertexFlags)(_flags | flag); }
    void removeFlag(VertexFlags flag) { _flags = (VertexFlags)(_flags & ~flag); }
    VertexFlags flags() const { return _flags; }

    /**
     * Operator to treat the Vertex class as a Point3 instance. Returns
     * the position of the vertex.
     */
    operator Point3() { return _position; };

    QString toString();

    Vertex & operator=(const Vertex & v);

    /**
     * Equals operator.
     */
    bool operator==(const Vertex& v) const;

    /**
     * Equals operator.
     */
    bool operator!=(const Vertex& v) const;

    Iterator<Vertex> vertexIterator();


    friend class VertexIterator;
};

#endif