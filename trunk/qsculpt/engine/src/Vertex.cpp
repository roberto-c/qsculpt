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

QAtomicInt Vertex::NEXT_ID(1);

Vertex::Vertex() 
	: _position(Point3(0, 0, 0)),
	_normal(Vector3(0, 1, 0)),
	_color(Vector3(1, 1, 1)),
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

Vertex::VertexIterator Vertex::vertexIterator() 
{ 
	return Vertex::VertexIterator(this);
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