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
#include "Point3D.h"

class Vertex
{
	Point3 _position;
	Vector3 _normal;
	Vector3 _color;
	
public:
	Point3 getPosition() { return _position; }
};

#endif
