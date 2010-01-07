/*
 *  HEdge.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef HEDGE_H_
#define HEDGE_H_

class Face;
class Vertex;

class HEdge {
	static QAtomicInt NEXT_ID;
	
	int _id;
	HEdge *_next, *_pair;
	Vertex *_head;
	Face *_face;
	
public:
	HEdge();
	
	~HEdge();
	
	/**
	 * Gets / sets a pointer to the next HEdge structure.
	 */
	const HEdge* next() const { return _next; };
	HEdge* next() { return _next; }
	void setNext(HEdge* he) { _next = he; };
	
	/**
	 * Gets / sets a pointer to the pair of this HEdge structure.
	 */
	const HEdge* pair() const { return _pair; };
	HEdge* pair() { return _pair; };
	void setPair(HEdge* he) { _pair = he; };

	/**
	 * Gets / sets a pointer to the vertex this structure points to.
	 */
	const Vertex* head() const { return _head; };
	Vertex* head() { return _head; };
	void setHead(Vertex* v) { _head = v; };
	
	/**
	 * Gets / sets a pointer to a face this half edge belongs to.
	 */
	const Face* face() const { return _face; };
	Face* face() { return _face; };
	void setFace(Face* f) { _face = f; };
};

#endif
