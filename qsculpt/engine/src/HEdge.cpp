/*
 *  HEdge.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include "StdAfx.h"
#include "HEdge.h"

QAtomicInt HEdge::NEXT_ID(1);

HEdge::HEdge() 
	: _next(NULL), 
	_pair(NULL),
	_head(NULL),
	_face(NULL)
{
	_id = NEXT_ID.fetchAndAddRelaxed(1);
}

HEdge::~HEdge()
{
}
