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
#include <QtOpenGL>
#include "Point3D.h"
#include "Box.h"

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
    double hw = 100.0;
    double hh = 100.0;
    double hd = 100.0;
    
    QVector<int> vertexID(8);

    vertexID[0] = addVertex(new Vertex(Point3(-hw, hh,-hd), Vector3(-hw, hh,-hd)));
    vertexID[1] = addVertex(new Vertex(Point3( hw, hh,-hd), Vector3( hw, hh,-hd)));
    vertexID[2] = addVertex(new Vertex(Point3( hw,-hh,-hd), Vector3( hw,-hh,-hd)));
    vertexID[3] = addVertex(new Vertex(Point3(-hw,-hh,-hd), Vector3(-hw,-hh,-hd)));

    vertexID[4] = addVertex(new Vertex(Point3(-hw, hh, hd), Vector3(-hw, hh, hd)));
    vertexID[5] = addVertex(new Vertex(Point3( hw, hh, hd), Vector3( hw, hh, hd)));
    vertexID[6] = addVertex(new Vertex(Point3( hw,-hh, hd), Vector3( hw,-hh, hd)));
    vertexID[7] = addVertex(new Vertex(Point3(-hw,-hh, hd), Vector3(-hw,-hh, hd)));
    
    QVector<int> indexList(4);
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

