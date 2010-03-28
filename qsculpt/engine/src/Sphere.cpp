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
    : Subdivision(),
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
#define NUM_LAT 32
#define NUM_PAR 15
	float lat_step = M_PI * 2 / NUM_LAT;
	float par_step = M_PI  / 16;
	float s = M_PI, u = 0;
	Point3 vtx, tmp;

	for ( s = 0; s < M_PI * 2; s+=lat_step) 
	{
		for ( u = par_step; u < M_PI; u+=par_step)
		{
			addVertex(evalCoords(s, u));
		}
	}
	int num_points = this->getPointList().size();
	QVector<int> indexList(4);
	for (int j = 0; j < NUM_LAT; ++j ) {
		for (int i = 0; i < NUM_PAR-1 ; ++i) {
			indexList[0] = ((i + j*NUM_PAR) ) % num_points;
			indexList[1] = ((i + j*NUM_PAR) + 1) % num_points;
			indexList[2] = ((i + j*NUM_PAR) + NUM_PAR + 1 ) % num_points;
			indexList[3] = ((i + j*NUM_PAR) + NUM_PAR ) % num_points;
			addFace( indexList );
		}
	}
	
	int topPointIndex = addVertex(evalCoords(0, 0));
	for (int j = 0; j < NUM_LAT; ++j ) {
		indexList[0] = ((j*NUM_PAR) ) % num_points;
		indexList[1] = ((j+1)*NUM_PAR) % num_points;
		indexList[2] = topPointIndex;
		indexList[3] = topPointIndex;
		addFace( indexList );
	}
	int bottomPointIndex = addVertex(evalCoords(0, M_PI));
	for (int j = 0; j < NUM_LAT; ++j ) {
		indexList[0] = ((j)*NUM_PAR + NUM_PAR - 1) % num_points;
		indexList[1] = ((j+1)*NUM_PAR  + NUM_PAR - 1) % num_points;
		indexList[2] = bottomPointIndex;
		indexList[3] = bottomPointIndex;
		addFace( indexList );
	}
}

Point3 Sphere::evalCoords(float s, float u) 
{
	Point3 vtx;
	
	vtx[0] = cosf(s) * sinf(u);
	vtx[1] = sinf(s) * sinf(u);
	vtx[2] = cosf(u);
	
	return vtx;
}

