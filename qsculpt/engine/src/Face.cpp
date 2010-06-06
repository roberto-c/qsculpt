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

QAtomicInt Face::NEXT_ID(1);

Face::Face(ISurface *surface) 
:	_surface(surface),
    _he(NULL),
	_vertex(NULL),
	midPoint(-1),
	isMarked(false),
	hashValue(0),
	_children(NULL)
{
	_id = NEXT_ID.fetchAndAddRelaxed(1);
}

Face::Face(ISurface *surface, const QVector<int>& vertexIndexList)
:   _surface(surface),
    _he(NULL),
	_vertex(NULL),
//point(vertexIndexList),
	midPoint(-1),
	isMarked(false),
	hashValue(0),
	_children(NULL)
{
	_id = NEXT_ID.fetchAndAddRelaxed(1);
	//normal.fill(-1, point.size());
	for(int i = 0; i < vertexIndexList.size(); ++i)
		hashValue += vertexIndexList[i];
}

Face::~Face()
{
	delete [] _children;
}

uint Face::hashCode() const
{
	if (!_he)
		return qHash(_id);
	Vector3 v;
	Iterator<Vertex> it = _he->vertexIterator();
	while(it.hasNext())
	{
		v += it.next();
	}
	return qHash(v);
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

void Face::addVertex(const QVector<Vertex*> v)
{
    NOT_IMPLEMENTED
}

void Face::setHEdge(Edge* hedge)
{
	_he = hedge;
}

Face::operator Point3() {
	NOT_IMPLEMENTED
	//return _he->head()->position();
	return Point3();
}

Face::operator Point3() const {
	NOT_IMPLEMENTED
	//return _he->head()->position();
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

void Face::subdivide(int level)
{
    
}

// VertexIterator

Face::VertexIterator::VertexIterator(const Face* f) 
:	_f(f),
	_e(NULL)
{
}

bool Face::VertexIterator::hasNext() const
{
    // true if the face has an edge assigned and
    // _e pointer hasn't been set yet (next() hasn't been called)
    // or _e has been set and _e is different to the initial
    // edget pointer (_f->_he)
	return _f->_he && (!_e || (_e->next() && _e->next() != _f->_he));
}


bool Face::VertexIterator::hasPrevious() const
{
	return false;
}


Vertex & Face::VertexIterator::next()
{
	//NOT_IMPLEMENTED
    _e = _e == NULL ? _f->_he : _e->next();
	return *_e->head();
}

const Vertex & Face::VertexIterator::next() const
{
	//NOT_IMPLEMENTED
	
	_e = _e == NULL ? _f->_he : _e->next();
	return *_e->head();
}

Vertex & Face::VertexIterator::previous()
{
	NOT_IMPLEMENTED
	//return *(_f->_he->head());
	static Vertex v;
	return v;
}

const Vertex & Face::VertexIterator::previous() const
{
	NOT_IMPLEMENTED
	//return *(_f->_he->head());
	static Vertex v;
	return v;
}
