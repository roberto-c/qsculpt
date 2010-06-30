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
    static QAtomicInt NEXT_ID;

    int _id;
    int _depth;
    ISurface *_surface;
    Edge *_he;
    Vertex* _vertex;
    Face* _children;

public:
    int midPoint;
    bool isMarked;
    qint32 hashValue;

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
    Edge* hedge() { return _he; };
    void setHEdge(Edge* hedge);

    /**
         * Sets the first point index reference.
     *
     * @param p1 index of the first point.
     * @param p2 index of the second point.
     * @param p3 index of the third point.
     */
    void setPoints(const QVector<int>& vertexIndexList) {
        NOT_IMPLEMENTED;
        //normal.resize(point.size());
        hashValue = 0;
        for(int i = 0; i < vertexIndexList.size(); ++i)
            hashValue += vertexIndexList[i];
    }

    bool hasEdge(const Edge& /*e*/) const {
        NOT_IMPLEMENTED;
        //        return hasEdge(e.head(), e.tail());
        return false;
    }

    bool hasEdge(int /*v1*/, int /*v2*/) const {
        NOT_IMPLEMENTED;
        return false;
        //        bool res = false;
        //        if (int index = point.indexOf(v1) != -1)
        //        {
        //            if (index == point.size() - 1)
        //                res = v2 == point[0];
        //            else
        //                res = v2 == point[index + 1];
        //        }
        //        return res;
    }

    bool operator==(const Face& t) const;

    operator Point3() const ;
    operator Point3();

    Iterator<Vertex> vertexIterator();
    Iterator<Vertex> constVertexIterator() const;

protected:
    void subdivide(int level);

    /* Inner classes */
    class VertexIterator : public IIterator<Vertex>
    {
        friend class Face;

        const Face* _f;
        mutable Edge* _e;

    protected:
        /**
         * Constructor of a vertex iterator. The vertex iterator
         * is only contructed by means of Vertex::vertexIterator() function
         */
        VertexIterator(const Face* f);

    public:
        /**
         * Return true if the iterator has more elements (i.e. it is not at the
         * end)
         */
        bool hasNext() const;

        /**
         * Returns true if the iterator is not at the beginning of the iteration
         */
        bool hasPrevious() const;

        /**
         * Returns the next element and advance the iterator by one.
         */
        Vertex & next();

        /**
         * Returns the next element and advance the iterator by one.
         */
        const Vertex & next() const;

        /**
         * Returns the previous elements and move the iterator one position
         * backwards.
         */
        Vertex & previous();

        /**
         * Returns the previous elements and move the iterator one position
         * backwards.
         */
        const Vertex & previous() const;
    };

    friend class VertexIterator;
};

#endif
