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
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/subdivision/Box.h>

using core::subdivision::Vertex;
using core::subdivision::Face;

Box::Box()
    : Subdivision()
{
    initPoints();
}

Box::~Box()
{
}

void Box::initPoints()
{
    //qDebug("Box::initPoints()");
    float hw = 1.0;
    float hh = 1.0;
    float hd = 1.0;
    
    std::vector<size_t> vertexID(8);
    vertexID[0] = addVertex(Point3(-hw, hh,-hd));
    vertexID[1] = addVertex(Point3( hw, hh,-hd));
    vertexID[2] = addVertex(Point3( hw,-hh,-hd));
    vertexID[3] = addVertex(Point3(-hw,-hh,-hd));
    vertex(vertexID[0])->cast<const Vertex*>()->normal() = Vector3(-hw, hh,-hd).normalized();
    vertex(vertexID[1])->cast<const Vertex*>()->normal() = Vector3( hw, hh,-hd).normalized();
    vertex(vertexID[2])->cast<const Vertex*>()->normal() = Vector3( hw,-hh,-hd).normalized();
    vertex(vertexID[3])->cast<const Vertex*>()->normal() = Vector3(-hw,-hh,-hd).normalized();
    vertex(vertexID[0])->cast<const Vertex*>()->texcoords() = Point2(0,0);
    vertex(vertexID[1])->cast<const Vertex*>()->texcoords() = Point2(1,0);
    vertex(vertexID[2])->cast<const Vertex*>()->texcoords() = Point2(1,1);
    vertex(vertexID[3])->cast<const Vertex*>()->texcoords() = Point2(0,1);
    
    vertexID[4] = addVertex(Point3(-hw, hh, hd));
    vertexID[5] = addVertex(Point3( hw, hh, hd));
    vertexID[6] = addVertex(Point3( hw,-hh, hd));
    vertexID[7] = addVertex(Point3(-hw,-hh, hd));
    vertex(vertexID[4])->cast<const Vertex*>()->normal() = Vector3(-hw, hh, hd).normalized();
    vertex(vertexID[5])->cast<const Vertex*>()->normal() = Vector3( hw, hh, hd).normalized();
    vertex(vertexID[6])->cast<const Vertex*>()->normal() = Vector3( hw,-hh, hd).normalized();
    vertex(vertexID[7])->cast<const Vertex*>()->normal() = Vector3(-hw,-hh, hd).normalized();
    vertex(vertexID[4])->cast<const Vertex*>()->texcoords() = Point2(0,0);
    vertex(vertexID[5])->cast<const Vertex*>()->texcoords() = Point2(1,0);
    vertex(vertexID[6])->cast<const Vertex*>()->texcoords() = Point2(1,1);
    vertex(vertexID[7])->cast<const Vertex*>()->texcoords() = Point2(0,1);
    
    Iterator<VertexHandle> it = vertexIterator();
    while (it.hasNext()) {
        static_cast<Vertex*>(it.next())->addFlag(VF_Crease);
    }
    
    std::vector<size_t> indexList(4);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[1];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[3];
    addFace( indexList );
    indexList[0] = vertexID[4];
    indexList[1] = vertexID[7];
    indexList[2] = vertexID[6];
    indexList[3] = vertexID[5];
    addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[7];
    indexList[3] = vertexID[4];
    addFace( indexList );
    indexList[0] = vertexID[5];
    indexList[1] = vertexID[6];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[1];
    addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[4];
    indexList[2] = vertexID[5];
    indexList[3] = vertexID[1];
    addFace( indexList );
    indexList[0] = vertexID[7];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[6];
    addFace( indexList );
}


Plane::Plane()
: Subdivision()
{
    initPoints(1, 1);
}

Plane::Plane(int m, int n)
: Subdivision()
{
    initPoints(m, n);
}

Plane::~Plane()
{
}
void Plane::initPoints()
{
    
}

void Plane::initPoints(int m, int n)
{
#define CELL(x,y) (((y)*(m+1))+(x))
    
    if ( m <= 0 || n <= 0) return;
    
    //qDebug("Box::initPoints()");
    float hw = 2.0;
    float hh = 2.0;
    float wstep = hw / m;
    float hstep = hh / n;
    float x=0, y=0;
    int j=0,i=0, numVtx=0;
    

    std::vector<size_t> vertexID((m+1) * (n+1));
    y = -hh / 2;
    float v = 0, u = 0;
    for (i = 0; i <= m; ++i) {
        x = -hw/2;
        u = 0;
        for (j = 0; j <= n; ++j) {
            vertexID[numVtx] = addVertex(Point3(x, 0, y));
            static_cast<Vertex*>(vertex(vertexID[0]))->texcoords() = Point2(u,v);
            numVtx++;
            x += wstep;
            u += 1/m;
        }
        y += hstep;
        v += 1/n;
    }
    std::vector<size_t> indexList(4);
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            indexList[0] = vertexID[CELL(j,i)];
            indexList[1] = vertexID[CELL(j+1,i)];
            indexList[2] = vertexID[CELL(j+1,i+1)];
            indexList[3] = vertexID[CELL(j,i+1)];
            addFace( indexList );
        }
    }
#undef CELL
}


