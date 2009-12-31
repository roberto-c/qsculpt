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
public:
	HEdge() 
	  : _next(NULL), 
		_pair(NULL),
		_head(NULL),
		_face(NULL) {
	};
	
	~HEdge() {
	};
	
	const HEdge* next() const { return _next; };
	void setNext(HEdge* he) { _next = he; };
	
	const HEdge* pair() const { return _pair; };
	void setPair(HEdge* he) { _pair = he; };

	const Vertex* head() const { return _head; };
	void setHead(Vertex* v) { _head = v; };
	
	const Face* face() const { return _face; };
	void setFace(Face* f) { _face = f; };
	
private:
	HEdge *_next, *_pair;
	Vertex *_head;
	Face *_face;
};

#endif
