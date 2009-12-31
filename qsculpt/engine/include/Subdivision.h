/*
 *  Subdivision.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <vector>
#include "Object3D.h"

class Vertex;
class HEdge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Subdivision : public Object3D
{
	typedef std::vector<Vertex*> VertexCollection;
	typedef std::vector<HEdge*> EdgesCollection;
	typedef std::vector<Face*> FacesCollection;
	
	/** Vertices by each subdivision level */
	std::vector<VertexCollection> _vertices;
	/** Edges by each subdivision level */
	std::vector<EdgesCollection> _edges;
	
	SubdivisionScheme* scheme;
};

#endif
