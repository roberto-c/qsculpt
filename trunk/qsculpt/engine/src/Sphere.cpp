/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#include "StdAfx.h"
#include "Sphere.h"
#include <QtOpenGL>
#include <math.h>

Sphere::Sphere()
    : Object3D(),
    m_pquadratic(0)
{
    initPoints();
}


Sphere::~Sphere()
{
    //gluDeleteQuadric(m_pquadratic);
}

void Sphere::initPoints()
{

    float x, y, z;

    float hw = 1.0 / 2;
    float hh = 1.0 / 2;
    float hd = 1.0 / 2;

	addVertex(Vertex( 0, 0, hd));
	addVertex(Vertex( 0, 0,-hd));

    addVertex(Vertex(-hw,-hh, 0.0));
    addVertex(Vertex( hw,-hh, 0.0));
    addVertex(Vertex( hw, hh, 0.0));
    addVertex(Vertex(-hw, hh, 0.0));
	
	QVector<int> indexList(4);
    indexList[0] = 2;
    indexList[1] = 3;
    indexList[2] = 4;
    indexList[3] = 0;
    addFace( indexList );
	indexList[0] = 2;
    indexList[1] = 0;
    indexList[2] = 4;
    indexList[3] = 5;
    addFace( indexList );
	indexList[0] = 3;
    indexList[1] = 2;
    indexList[2] = 5;
    indexList[3] = 1;
    addFace( indexList );
	indexList[0] = 5;
    indexList[1] = 4;
    indexList[2] = 3;
    indexList[3] = 1;
    addFace( indexList );
	
	qDebug() << "Width: " << getWidth();
	qDebug() << "Height: " << getHeight();
	qDebug() << "Depth: " << getDepth();
	
    //subdivide();
}

