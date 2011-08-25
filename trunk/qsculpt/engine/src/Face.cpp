/*
 *  Face.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 1/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "StdAfx.h"
#include "Face.h"
#include "HEdge.h"
#include "ISurface.h"

class Face::VertexIterator : public IIterator<Vertex>
{
    friend class Face;

    const Face* _f;
    mutable Edge* _e;
    mutable bool first_;

protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Face::vertexIterator() function
     */
    VertexIterator(const Face* f);

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
     * Return the next element without advancing to the iterator
     */
    Vertex & peekNext();
    
    /**
     * Return the next element without advancing to the iterator
     */
    const Vertex & peekNext() const;

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
     * Returns the previous element.
     */
    Vertex & peekPrevious();
    
    /**
     * Returns the previous element.
     */
    const Vertex & peekPrevious() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};


class Face::EdgeIterator : public IIterator<Edge>
{
    friend class Face;

    const Face* _f;
    mutable Edge* _e;

protected:
    /**
     * Constructor of a vertex iterator. The Edge iterator
     * is only contructed by means of Face::edgeIterator() function
     */
    EdgeIterator(const Face* f);

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
     * Return the next element without advancing to the iterator
     */
    Edge & peekNext();
    
    /**
     * Return the next element without advancing to the iterator
     */
    const Edge & peekNext() const;
    
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
     * Returns the previous element.
     */
    Edge & peekPrevious();
    
    /**
     * Returns the previou element
     */
    const Edge & peekPrevious() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

//QAtomicInt Face::NEXT_ID(0);
static QAtomicInt NEXT_ID(1);

Face::Face(ISurface *surface)
    :   _flags(FF_None),
    _surface(surface),
    _he(NULL),
    _vertex(NULL),
    _children(NULL),
    _nVertices(0),
    _userData(NULL)
//hashValue(0)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
}

Face::Face(ISurface *surface, const QVector<int>& vertexIndexList)
    :   _flags(FF_None),
    _surface(surface),
    _he(NULL),
    _vertex(NULL),
    //point(vertexIndexList),
    _children(NULL),
    _nVertices(0),
    _userData(NULL)
//    hashValue(0)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
}

Face::~Face()
{
    delete [] _children;
}

// TODO: implement a correct and fast hash
uint Face::hashCode() const
{
    return qHash(_id);
    
//    if (!_he)
//        return qHash(_id);
//    Vector3 v;
//    Iterator<Vertex> it = _he->vertexIterator();
//    while(it.hasNext())
//    {
//        v = v + (Vector3)it.next();
//    }
//    return qHash(v);
}

bool Face::isValid() {
    return true;
    //	return !point.isEmpty();
}

Face& Face::addEdge(Edge* he)
{
    if (!_he) {
        _he = he;
    } else {
        Iterator<Edge> it = _he->hedgeIterator();
        while(it.hasNext())
            it.next();
    }
    return *this;
}

void Face::addVertex(const QVector<Vertex*> /*v*/)
{
    NOT_IMPLEMENTED
}

void Face::setHEdge(Edge* hedge)
{
    _he = hedge;
}

size_t Face::numVertices() const {
    size_t n = 0;
    Iterator<Vertex> it = constVertexIterator();
    while (it.hasNext()) {
        n += 1;
        it.next();
    }
    return n;
}


void Face::setPoints(const QVector<int>& vertexIndexList) {
    throw std::runtime_error("Not implemented");
}

bool Face::hasEdge(const Edge& /*e*/) const {
    NOT_IMPLEMENTED;
    return false;
}

bool Face::hasEdge(int /*v1*/, int /*v2*/) const {
    NOT_IMPLEMENTED;
    return false;
}

Face::operator Point3()
{
    NOT_IMPLEMENTED;

    return Point3();
}

Face::operator Point3() const
{
    NOT_IMPLEMENTED
    return Point3();
}


bool Face::operator==(const Face& t) const {
    if (hashCode() != t.hashCode())
        return false;
    Iterator<Vertex> it = constVertexIterator();
    Iterator<Vertex> it2 = t.constVertexIterator();
    while(it.hasNext()) {
        if (!it2.hasNext())
            return false;
        Vertex* v = & it.next();
        Vertex* v2 = & it2.next();
        if (v->iid() != v2->iid())
            return false;
    }
    return true;
}

Iterator<Vertex> Face::vertexIterator()
{
    return Iterator<Vertex>(new Face::VertexIterator(this));
}

Iterator<Vertex> Face::constVertexIterator() const
{
    return Iterator<Vertex>(new Face::VertexIterator(this));
}

Iterator<Edge> Face::edgeIterator()
{
    return Iterator<Edge>(new Face::EdgeIterator(this));
}

Iterator<Edge> Face::constEdgeIterator() const
{
    return Iterator<Edge>(new Face::EdgeIterator(this));
}

void Face::subdivide(int /*level*/)
{

}

// VertexIterator

Face::VertexIterator::VertexIterator(const Face* f)
    :	_f(f),
    _e(f->_he),
    first_(true)
{
}

IIterator<Vertex>* Face::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_f);
    it->_e = _e;
    return it;
}

bool Face::VertexIterator::hasNext() const
{
    // true if the face has an edge assigned and
    // _e pointer hasn't been set yet (next() hasn't been called)
    // or _e has been set and _e is different to the initial
    // edget pointer (_f->_he)
    return (first_ && _e != NULL) || (_e != NULL && _e != _f->_he);
}


bool Face::VertexIterator::hasPrevious() const
{
    return false;
}


Vertex & Face::VertexIterator::next()
{
    assert(_e != NULL);
    first_ = false;
    Vertex * v = _e->head();
    _e = _e->next();
    return *v;
}

const Vertex & Face::VertexIterator::next() const
{
    assert(_e != NULL);
    first_ = false;
    Vertex * v = _e->head();
    _e = _e->next();
    return *v;
}

Vertex & Face::VertexIterator::peekNext()
{
    return *_e->head();
}

const Vertex & Face::VertexIterator::peekNext() const
{
    return *_e->head();
}

Vertex & Face::VertexIterator::previous()
{
    throw std::logic_error("Not implemented");
}

const Vertex & Face::VertexIterator::previous() const
{
    throw std::logic_error("Not implemented");
}

Vertex & Face::VertexIterator::peekPrevious()
{
    throw std::logic_error("Not implemented");
}

const Vertex & Face::VertexIterator::peekPrevious() const
{
    throw std::logic_error("Not implemented");
}

bool Face::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    bool res = false;
    switch(origin)
    {
    case Iter_Start:
        {
            _e = NULL;
            while(pos && hasNext()){
                next();
                --pos;
            }
            res = true;
        }
        break;
    default:
        res = false;
    }
    return res;
}



// EdgeIterator

Face::EdgeIterator::EdgeIterator(const Face* f)
    :	_f(f),
    _e(NULL)
{
}

IIterator<Edge>* Face::EdgeIterator::clone() const
{
    EdgeIterator* it = new EdgeIterator(_f);
    it->_e = _e;
    return it;
}

bool Face::EdgeIterator::hasNext() const
{
    // true if the face has an edge assigned and
    // _e pointer hasn't been set yet (next() hasn't been called)
    // or _e has been set and _e is different to the initial
    // edget pointer (_f->_he)
    return _f->_he && (!_e || (_e->next() && _e->next() != _f->_he));
}


bool Face::EdgeIterator::hasPrevious() const
{
    return false;
}


Edge & Face::EdgeIterator::next()
{
    //NOT_IMPLEMENTED
    _e = _e == NULL ? _f->_he : _e->next();
    assert(_e != NULL);
    return *_e;
}

const Edge & Face::EdgeIterator::next() const
{
    //NOT_IMPLEMENTED

    _e = _e == NULL ? _f->_he : _e->next();
    assert(_e != NULL);
    return *_e;
}

Edge & Face::EdgeIterator::peekNext()
{
    //NOT_IMPLEMENTED
    return _e==NULL ? *(_f->_he) : *(_e->next());
}

const Edge & Face::EdgeIterator::peekNext() const
{
    //NOT_IMPLEMENTED
    return _e==NULL ? *(_f->_he) : *(_e->next());
}

Edge & Face::EdgeIterator::previous()
{
    throw std::logic_error("Not implemented");
}

const Edge & Face::EdgeIterator::previous() const
{
    throw std::logic_error("Not implemented");
}

Edge & Face::EdgeIterator::peekPrevious()
{
    throw std::logic_error("Not implemented");
}

const Edge & Face::EdgeIterator::peekPrevious() const
{
    throw std::logic_error("Not implemented");
}

bool Face::EdgeIterator::seek(int pos, IteratorOrigin origin) const
{
    bool res = false;
    switch(origin)
    {
    case Iter_Start:
        {
            _e = NULL;
            while(pos && hasNext()){
                next();
                --pos;
            }
            res = true;
        }
        break;
    default:
        res = false;
    }
    return res;
}
