/*
 *  Vertex.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VERTEX_H
#define VERTEX_H

#include <Eigen/Core>
#include <QAtomicInt>
#include "Point3D.h"

class HEdge;

class Vertex
{
	static QAtomicInt NEXT_ID;
	
	int _id;
	Point3 _position;
	Vector3 _normal;
	Vector3 _color;
	
	HEdge *_he;
	
public:
	class VertexIterator;
	
public:
	
	/**
	 * Contructs a new vertex instance
	 */
	Vertex();
	
	/**
	 * Copy constructor. Creates a new instance of a vertex
	 * with the same attributes as another vertex.
	 *
	 * The new vertex have the same half edge assigned as the original
	 * vertex.
	 */
	Vertex(const Vertex&);
	
	/**
	 * Gets / sets the position of the vertex
	 */
	Point3 getPosition() const { return _position; };
	Point3 position() const { return _position; };
	Point3& position() { return _position; };
		
	/**
	 * Get / sets the color attribute of the vertex;
	 */
	Vector3 color() const { return _color; }
	Vector3& color() { return _color; }

	/**
	 * Gets / sets the normal attribute for this vertex instance
	 */
	Vector3 normal() const { return _normal; }
	Vector3& normal() { return _normal; }
	
	/**
	 * Assign or returns one of the half edges associated to the vertex.
	 */
	void setHedge(HEdge *he) { _he = he; }
	const HEdge* getHedge() const { return _he; }
	HEdge* getHedge() { return _he; }
	
	/**
	 * Operator to treat the Vertex class as a Point3 instance. Returns
	 * the position of the vertex.
	 */
	operator Point3() { return _position; };
	
	QString toString();
	
	Vertex & operator=(const Vertex & v);
	
	VertexIterator vertexIterator();
	
	/* Inner classes */
public:
	class VertexIterator
	{
		friend class Vertex;
		
		Vertex* _v;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		VertexIterator(Vertex* v);
		
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
