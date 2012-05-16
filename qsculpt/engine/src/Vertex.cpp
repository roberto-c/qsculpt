/*
 *  Vertex.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 1/6/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "StdAfx.h"
#include "Vertex.h"
#include "HEdge.h"
#include "Face.h"


class Vertex::VertexIterator : public IIterator<Vertex>
{
public:
    typedef Vertex::SharedPtr   SharedPtr;
    typedef Vertex::WeakPtr     WeakPtr;
    typedef Vertex::Ptr         Ptr;
    
private:
    friend class Vertex;

    Vertex::SharedPtr         _v;
    mutable Edge::SharedPtr   _iniHe;
    mutable Edge::SharedPtr   _nextHe;
    mutable bool        _firstIt;

protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(Vertex* v);

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
    SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    SharedPtr peekNext();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr peekNext() const;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const;

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class Vertex::FaceIterator : public IIterator<Face>
{
public:
    typedef Face::SharedPtr   SharedPtr;
    typedef Face::WeakPtr     WeakPtr;
    typedef Face::Ptr         Ptr;
    
private:

    friend class Vertex;
    
    Vertex::SharedPtr       _v;
    mutable Edge::SharedPtr _iniHe;
    mutable Edge::SharedPtr _nextHe;
    mutable bool            _firstIt;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(Vertex* v);
    
public:
    /**
     *
     */
    IIterator<Face>* clone() const;
    
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
    SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    SharedPtr peekNext();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr peekNext() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous();
        
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class Vertex::EdgeIterator : public IIterator<Edge>
{
public:
    typedef Edge::SharedPtr     SharedPtr;
    typedef Edge::WeakPtr       WeakPtr;
    typedef Edge::Ptr           Ptr;
    
private:
    friend class Vertex;
    
    Vertex::SharedPtr           _v;
    mutable Edge::SharedPtr     _iniHe;
    mutable Edge::SharedPtr     _nextHe;
    mutable bool                _firstIt;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator(Vertex* v);
    
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
    SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    SharedPtr peekNext();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr peekNext() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};


QAtomicInt Vertex::NEXT_ID(1);

Vertex::Vertex() 
    : _position(Point3(0, 0, 0)),
    _normal(Vector3(0, 1, 0)),
    _color(Vector3(1, 1, 1)),
    _flags(VF_None),
    _he(NULL),
    _userData(NULL)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
}

Vertex::Vertex(const Point3 & position, 
               const Vector3 & normal,
               const Vector3 & color)
                   : _position(position),
                   _normal(normal.normalized()),
                   _color(color),
                   _flags(VF_None),
                   _he(NULL),
                   _userData(NULL)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
}

Vertex::Vertex(const Vertex& v)
{
    _id = NEXT_ID.fetchAndAddRelaxed(1);
    _position = v._position;
    _normal = v._normal;
    _color = v._color;
    _flags = v._flags;
    _he = v._he;
    _userData = v._userData;
}

Vertex & Vertex::operator=(const Vertex & v)
                          {
    if ( this == &v )
        return *this;

    _position = v._position;
    _normal = v._normal;
    _color = v._color;
    _he = v._he;

    return *this;
}

bool Vertex::operator==(const Vertex& v) const
{
    if (this == &v)
        return true;

    return (_position == v._position && _normal == v._normal
            && _color == v._color);
}

bool Vertex::operator!=(const Vertex& v) const
{
    return !operator==(v);
}

Iterator<Vertex> Vertex::vertexIterator()
{ 
    return Iterator<Vertex>(new VertexIterator(this));
}

Iterator<Face> Vertex::faceIterator()
{ 
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Edge> Vertex::edgeIterator()
{ 
    return Iterator<Edge>(new EdgeIterator(this));
}

QString Vertex::toString()
{
    QString str = QString("V_%1:(%2, %3, %4)")
                  .arg(_id)
                  .arg(_position.x())
                  .arg(_position.y())
                  .arg(_position.z());

    return str;
}

// VertexIterator

Vertex::VertexIterator::VertexIterator(Vertex* v) 
:	_iniHe(v ? v->edge() : NULL),
    _nextHe(NULL),
    _firstIt(true)
{
    _nextHe = _iniHe ; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL;
}

IIterator<Vertex>* Vertex::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(NULL);
    it->_nextHe = _nextHe;
    it->_iniHe = _iniHe;
    it->_firstIt = _firstIt;
    return it;
}

bool Vertex::VertexIterator::hasNext() const
{
    return (_firstIt ) || ( (_nextHe != NULL) && (_iniHe != _nextHe) ) ;
}


bool Vertex::VertexIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
    return false;
}

Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Vertex * v = _nextHe->head();
    _nextHe = _nextHe->pair()->next();
    return v;
}

const Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Vertex * v = _nextHe->head();
    _nextHe = _nextHe->pair()->next();
    return v;
}

Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::peekNext()
{
    return _nextHe->head();
}

const Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::peekNext() const
{
    return _nextHe->head();
}

Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::previous()
{
    NOT_IMPLEMENTED
}

const Vertex::VertexIterator::SharedPtr Vertex::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Vertex::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    return false;
}

// FaceIterator

Vertex::FaceIterator::FaceIterator(Vertex* v) 
:	_iniHe(v ? v->edge() : NULL),
    _nextHe(NULL),
    _firstIt(true)
{
    _nextHe = _iniHe ; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL && _nextHe->face() != NULL;
}

IIterator<Face>* Vertex::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(NULL);
    it->_nextHe = _nextHe;
    it->_iniHe = _iniHe;
    it->_firstIt = _firstIt;
    return it;
}

bool Vertex::FaceIterator::hasNext() const
{    
    return (_firstIt ) || ( (_nextHe != NULL) && (_iniHe != _nextHe) && (_nextHe->face() != NULL) ) ;
}


bool Vertex::FaceIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
}


Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Face * f = _nextHe->face();
    _nextHe = _nextHe->pair()->next();
    return f;    
}

const Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Face * f = _nextHe->face();
    _nextHe = _nextHe->pair()->next();
    return f;    
}

Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::peekNext()
{
    return _nextHe->face();
}

const Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::peekNext() const
{
    return _nextHe->face();
}

Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::previous()
{
    NOT_IMPLEMENTED
}

const Vertex::FaceIterator::SharedPtr Vertex::FaceIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Vertex::FaceIterator::seek(int pos, IteratorOrigin origin) const
{
    NOT_IMPLEMENTED
    return false;
}

// EdgeIterator

Vertex::EdgeIterator::EdgeIterator(Vertex* v) 
:	_iniHe(v ? v->edge() : NULL),
_nextHe(NULL),
_firstIt(true)
{
    _nextHe = _iniHe ; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL && _nextHe->face() != NULL;
}

IIterator<Edge>* Vertex::EdgeIterator::clone() const
{
    EdgeIterator *it = new EdgeIterator(NULL);
    it->_nextHe = _nextHe;
    it->_iniHe = _iniHe;
    it->_firstIt = _firstIt;
    return it;
}

bool Vertex::EdgeIterator::hasNext() const
{    
    return (_firstIt ) || ( (_nextHe != NULL) && (_iniHe != _nextHe) && (_nextHe->face() != NULL) ) ;
}


bool Vertex::EdgeIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
}


Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Edge * e = _nextHe;
    _nextHe = _nextHe->pair()->next();
    return e;
}

const Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Edge * e = _nextHe;
    _nextHe = _nextHe->pair()->next();
    return e;
}

Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::peekNext()
{
    return _nextHe;
}

const Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::peekNext() const
{
    return _nextHe;
}

Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::previous()
{
    NOT_IMPLEMENTED
}

const Vertex::EdgeIterator::SharedPtr Vertex::EdgeIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Vertex::EdgeIterator::seek(int pos, IteratorOrigin origin) const
{
    NOT_IMPLEMENTED
    return false;
}
