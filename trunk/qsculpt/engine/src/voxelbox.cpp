/*
 *  voxelbox.cpp
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 2/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "stdafx.h"
#include <QtOpenGL>
#include "voxelbox.h"

VoxelBox::VoxelBox()
: VoxelObject()
{
    initPoints();
}

VoxelBox::~VoxelBox()
{
}

void VoxelBox::initPoints()
{
    qDebug("VoxelBox::initPoints()");
    
	for (int i = -50; i < 50; ++i)
		for (int j = -50; j < 50; ++j)
		{
			addPoint(Vertex(  i,   j,  49));
			addPoint(Vertex(  i,   j, -50));
			addPoint(Vertex(  i,  49,   j));
			addPoint(Vertex(  i, -50,   j));
			addPoint(Vertex( 49,   i,   j));
			addPoint(Vertex(-50,   i,   j));
		}
}
