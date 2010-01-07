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
#include "HEdge.h"

class Face {
	static QAtomicInt NEXT_ID;
	
	int _id;
	HEdge *_he;
	
public:
	class VertexIterator;
	
public:
	Face();
	
	~Face();
	
	operator Point3() {
		return _he->head()->position();
	}
	
	operator Point3() const {
		return _he->head()->position();
	}
	
	VertexIterator vertexIterator();
	
	/* Inner classes */
public:
	class VertexIterator
	{
		friend class Face;
		
		Face* _f;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		VertexIterator(Face* f);
		
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
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		Vertex & previous();
	};
	
	friend class VertexIterator;
};

#endif
