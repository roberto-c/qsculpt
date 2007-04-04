/***************************************************************************
*   Copyright (C) 2007 by Juan Roberto Cabral Flores   *
*   roberto.cabral@gmail.com   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
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
