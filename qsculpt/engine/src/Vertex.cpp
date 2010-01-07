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
	_normal(Vector3(0, 0, 0)),
	_color(Vector3(1, 1, 1)),
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

Vertex::VertexIterator::VertexIterator(Vertex* v) : _v(v)
{
}

bool Vertex::VertexIterator::hasNext() const
{
	return false;
}


bool Vertex::VertexIterator::hasPrevious() const
{
	return false;
}


Vertex & Vertex::VertexIterator::next()
{
	return *(_v->_he->head());
}

Vertex & Vertex::VertexIterator::previous()
{
	return *(_v->_he->head());
}