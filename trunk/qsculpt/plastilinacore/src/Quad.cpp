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
#include <PlastilinaCore/Quad.h>

Quad::Quad()
: Subdivision()
{
    initPoints();
    initTriangles();
}

Quad::~Quad()
{
}

void Quad::initPoints()
{
    //qDebug("Box::initPoints()");
}

void Quad::initTriangles()
{
    //qDebug("Box::initTriangles()");
    double hw = 1.0;
    double hh = 1.0;
    double hd = 0.0;

    std::vector<size_t> indexList(4);
    indexList[0] = addVertex(new Vertex(Point3(-hw, hh, hd), Vector3(-hw, hh, hd)));
    indexList[1] = addVertex(new Vertex(Point3( hw, hh, hd), Vector3( hw, hh, hd)));
    indexList[2] = addVertex(new Vertex(Point3( hw,-hh, hd), Vector3( hw,-hh, hd)));
    indexList[3] = addVertex(new Vertex(Point3(-hw,-hh, hd), Vector3(-hw,-hh, hd)));
    addFace( indexList );
    //qDebug("Box::initTriangles() end");
}
