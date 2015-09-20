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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/subdivision/Sphere.h>
#include <math.h>

using core::subdivision::Vertex;
using core::subdivision::Face;

Sphere::Sphere()
    : Subdivision(),
    m_pquadratic(0)
{
    initPoints();
}


Sphere::~Sphere()
{
}

void Sphere::initPoints()
{	
#define NUM_LAT 32U
#define NUM_PAR 16U

    typedef std::vector<size_t>::size_type size_type;
    
	float lat_step = M_PI * 2 / NUM_LAT;
	float par_step = M_PI  / NUM_PAR;
	float s = M_PI, u = 0;
	Point3 vtx, tmp;
    std::vector<size_t> vertexID;
    size_type i, j, index, num_points;

    for (size_t j = 1; j < NUM_LAT+1; ++j ) {
        s = j*lat_step;
		for (size_t i = 1; i < NUM_PAR; ++i) {
            u = i*par_step;
            vertexID.push_back(addVertex(evalCoords(s, u)));
        }
    }

    num_points = static_cast<size_type>(this->numVertices());
	std::vector<size_t> indexList(4);
	for (j = 0; j < NUM_LAT; ++j ) {
		for (i = 0; i < NUM_PAR-2 ; ++i) {
            index = i + j*(NUM_PAR-1);
			indexList[0] = vertexID[ index % num_points];
			indexList[1] = vertexID[(index + 1) % num_points];
			indexList[2] = vertexID[(index + NUM_PAR ) % num_points];
			indexList[3] = vertexID[(index + NUM_PAR - 1 ) % num_points];
			addFace( indexList );
		}
	}
    
    // adjust normals
    Iterator<VertexHandle> it = vertexIterator();
    while (it.hasNext()) {
        auto vtx = static_cast<Vertex*>(it.next());
        vtx->normal() = vtx->position();
        vtx->normal().normalize();
    }
	
    indexList.resize(3);
    vertexID.push_back(addVertex(evalCoords(0, 0)));
    size_type topPointIndex = vertexID.size() - 1;
	for (j = 0; j < NUM_LAT; ++j ) {
		indexList[0] = vertexID[((j*(NUM_PAR-1)) ) % num_points];
		indexList[1] = vertexID[((j+1)*(NUM_PAR-1)) % num_points];
		indexList[2] = vertexID[ topPointIndex];
		addFace( indexList );
	}
}

Point3 Sphere::evalCoords(float s, float u) 
{
	Point3 vtx;
	
	vtx[0] = cosf(s) * sinf(u);
	vtx[1] = cosf(u);
	vtx[2] = sinf(s) * sinf(u);
	
	return vtx;
}

