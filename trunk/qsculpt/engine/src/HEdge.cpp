/*
 *  HEdge.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "StdAfx.h"
#include "HEdge.h"
#include "Vertex.h"


class Edge::VertexIterator : public IIterator<Vertex>
{
    friend class Edge;

    Edge* _he;
    Edge* _currHe;

protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(Edge* he);

public:
    /**
     *
     */
    IIterator<Vertex>* clone() const;

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

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class Edge::HEdgeIterator : public IIterator<Edge>
{
    friend class Edge;

    Edge* _he;
    Edge* _currHe;

protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    HEdgeIterator(Edge* he);

public:
    /**
     *
     */
    IIterator<Edge>* clone() const;

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
    Edge & next();

    /**
     * Returns the next element and advance the iterator by one.
     */
    const Edge & next() const;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    Edge & previous();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const Edge & previous() const;

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

QAtomicInt Edge::NEXT_ID(1);

Edge::Edge()
    : _flags(EF_None),
    _next(NULL),
    _pair(NULL),
    _head(NULL),
    _tail(NULL),
    _face(NULL)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
}

Edge::Edge(Vertex* tail, Vertex* head)
    : _flags(EF_None),
    _next(NULL),
    _pair(NULL),
    _head(head),
    _tail(tail),
    _face(NULL)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
    assert(tail && head && tail->iid() != head->iid());
}

Edge::~Edge()
{
}

bool Edge::isEqual(const Edge& he) const
{
    if (this == &he)
        return true;

    //	if (_next == he._next && _pair == he._pair)
    //		return true;

    if (_head == he._head && _tail == he._tail)
        return true;

    if (_head->iid() == he._head->iid() &&
        _tail->iid() == he._tail->iid())
        return true;

    return false;
}

bool Edge::operator==(const Edge& he) const
{
    return isEqual(he);
}

bool Edge::operator!=(const Edge& he) const
{
    return !isEqual(he);
}

Iterator<Vertex> Edge::vertexIterator()
{
    return Iterator<Vertex>(new VertexIterator(this));
}

Iterator<Edge> Edge::hedgeIterator()
{
    return Iterator<Edge>(new HEdgeIterator(this));
}

/* Inner classes */

Edge::VertexIterator::VertexIterator(Edge* he)
    : _he(he)
{
    _currHe = _he;
}

IIterator<Vertex>* Edge::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_he);
    it->_currHe = _currHe;
    return it;
}

bool Edge::VertexIterator::hasNext() const
{
    NOT_IMPLEMENTED
}

bool Edge::VertexIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
}

Vertex & Edge::VertexIterator::next()
{
    NOT_IMPLEMENTED
}

const Vertex & Edge::VertexIterator::next() const
{
    NOT_IMPLEMENTED
}

Vertex & Edge::VertexIterator::previous()
{
    NOT_IMPLEMENTED
}

const Vertex & Edge::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Edge::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    return false;
}

Edge::HEdgeIterator::HEdgeIterator(Edge* he)
    : _he(he)
{
    _currHe = _he;
}

IIterator<Edge>* Edge::HEdgeIterator::clone() const
{
    HEdgeIterator *it = new HEdgeIterator(_he);
    it->_currHe = _he;
    return it;
}

bool Edge::HEdgeIterator::hasNext() const
{
    return _currHe != NULL;
}

bool Edge::HEdgeIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
}

Edge & Edge::HEdgeIterator::next()
{
    NOT_IMPLEMENTED
}

const Edge & Edge::HEdgeIterator::next() const
{
    NOT_IMPLEMENTED
}

Edge & Edge::HEdgeIterator::previous()
{
    NOT_IMPLEMENTED
}

const Edge & Edge::HEdgeIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Edge::HEdgeIterator::seek(int pos, IteratorOrigin origin) const
{
    NOT_IMPLEMENTED
    return false;
}
