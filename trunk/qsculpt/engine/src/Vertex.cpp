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


class Vertex::VertexIterator : public IIterator<Vertex>
{
    friend class Vertex;

    Vertex* _v;
    Edge* _currHe;

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
    Vertex & next();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    Vertex & previous();

    /**
     * Returns the next element and advance the iterator by one.
     */
    const Vertex & next() const;

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

QAtomicInt Vertex::NEXT_ID(1);

Vertex::Vertex() 
    : _position(Point3(0, 0, 0)),
    _normal(Vector3(0, 1, 0)),
    _color(Vector3(1, 1, 1)),
    _flags(VF_None),
    _he(NULL)
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
                   _he(NULL)
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
    :	_v(v),
    _currHe(NULL)
{
}

IIterator<Vertex>* Vertex::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_v);
    it->_currHe = _currHe;
    return it;
}

bool Vertex::VertexIterator::hasNext() const
{
    NOT_IMPLEMENTED
            //if (!_currHe)
            //	return false;

            return false;
}


bool Vertex::VertexIterator::hasPrevious() const
{
    return false;
}


Vertex & Vertex::VertexIterator::next()
{
    NOT_IMPLEMENTED
            //return *(_v->_he->head());
            return *_v;
}

const Vertex & Vertex::VertexIterator::next() const
{
    NOT_IMPLEMENTED
            //return *(_v->_he->head());
            return *_v;
}

Vertex & Vertex::VertexIterator::previous()
{
    NOT_IMPLEMENTED
            //return *(_v->_he->head());
            return *_v;
}

const Vertex & Vertex::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
            //return *(_v->_he->head());
            return *_v;
}

bool Vertex::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    return false;
}
