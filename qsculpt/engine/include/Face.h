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


class HEdge;

class Face {
public:
	Face() {
	}
	
	~Face() {
	}
	
	Point3 operator Point3() {
		return h->getVertex().getPosition();
	}
	
	const Point3 operator Point3() const {
		return h->getVertex().getPosition();
	}
	
private:
	HEdge *h;
	
};

#endif
