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

QAtomicInt Face::NEXT_ID(1);

Face::Face() : _he(NULL)
{
	_id = NEXT_ID.fetchAndAddRelaxed(1);
}

Face::~Face()
{
}


Face::VertexIterator Face::vertexIterator() 
{ 
	return Face::VertexIterator(this);
}


// VertexIterator

Face::VertexIterator::VertexIterator(Face* f) : _f(f)
{
}

bool Face::VertexIterator::hasNext() const
{
	return false;
}


bool Face::VertexIterator::hasPrevious() const
{
	return false;
}


Vertex & Face::VertexIterator::next()
{
	return *(_f->_he->head());
}

Vertex & Face::VertexIterator::previous()
{
	return *(_f->_he->head());
}