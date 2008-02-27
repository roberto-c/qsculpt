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

    double x, y, z;

    double hw = getWidth() / 2;
    double hh = getHeight() / 2;
    double hd = getDepth() / 2;

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
    indexList[3] = 5;
    addFace( indexList );
	
    //subdivide();
}

