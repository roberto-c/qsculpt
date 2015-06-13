/***************************************************************************
 *   Copyright (C) 2015 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
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
#include <PlastilinaCore/Plastilina.h>
#include "PrimitiveFactory.h"

#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include "GpuSubdivision.h"

namespace core {

GpuSubdivision *
PrimitiveFactory<GpuSubdivision>::createBox() {
    GpuSubdivision * surface = new GpuSubdivision;
    
    //qDebug("Box::initPoints()");
    double hw = 1.0;
    double hh = 1.0;
    double hd = 1.0;
    
    std::vector<size_t> vertexID(8);
    vertexID[0] = surface->addVertex(Point3(-hw, hh,-hd));
    vertexID[1] = surface->addVertex(Point3( hw, hh,-hd));
    vertexID[2] = surface->addVertex(Point3( hw,-hh,-hd));
    vertexID[3] = surface->addVertex(Point3(-hw,-hh,-hd));
    surface->vertex(vertexID[0])->texcoords() = Point2(0,0);
    surface->vertex(vertexID[1])->texcoords() = Point2(1,0);
    surface->vertex(vertexID[2])->texcoords() = Point2(1,1);
    surface->vertex(vertexID[3])->texcoords() = Point2(0,1);
    
    vertexID[4] = surface->addVertex(Point3(-hw, hh, hd));
    vertexID[5] = surface->addVertex(Point3( hw, hh, hd));
    vertexID[6] = surface->addVertex(Point3( hw,-hh, hd));
    vertexID[7] = surface->addVertex(Point3(-hw,-hh, hd));
    surface->vertex(vertexID[4])->texcoords() = Point2(0,0);
    surface->vertex(vertexID[5])->texcoords() = Point2(1,0);
    surface->vertex(vertexID[6])->texcoords() = Point2(1,1);
    surface->vertex(vertexID[7])->texcoords() = Point2(0,1);
    
    Iterator<Vertex> it = surface->vertexIterator();
    while (it.hasNext()) {
        it.next()->addFlag(VF_Crease);
    }
    
    std::vector<Vertex::size_t> indexList(4);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[1];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[3];
    surface->addFace( indexList );
    indexList[0] = vertexID[4];
    indexList[1] = vertexID[7];
    indexList[2] = vertexID[6];
    indexList[3] = vertexID[5];
    surface->addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[7];
    indexList[3] = vertexID[4];
    surface->addFace( indexList );
    indexList[0] = vertexID[5];
    indexList[1] = vertexID[6];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[1];
    surface->addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[4];
    indexList[2] = vertexID[5];
    indexList[3] = vertexID[1];
    surface->addFace( indexList );
    indexList[0] = vertexID[7];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[6];
    surface->addFace( indexList );

    return surface;
}

}; /* namespace core */
