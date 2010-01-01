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
    : Object3D()
{
    initPoints();
    initTriangles();
}

Box::~Box()
{
}

void Box::initPoints()
{
    //qDebug("Box::initPoints()");
    double hw = 1.0;
    double hh = 1.0;
    double hd = 1.0;

    addVertex(Point3(-hw, hh,-hd));
    addVertex(Point3( hw, hh,-hd));
    addVertex(Point3( hw,-hh,-hd));
    addVertex(Point3(-hw,-hh,-hd));

    addVertex(Point3(-hw, hh, hd));
    addVertex(Point3( hw, hh, hd));
    addVertex(Point3( hw,-hh, hd));
    addVertex(Point3(-hw,-hh, hd));
}

void Box::initTriangles()
{
    //qDebug("Box::initTriangles()");

    QVector<int> indexList(4);
    indexList[0] = 0;
    indexList[1] = 1;
    indexList[2] = 2;
    indexList[3] = 3;
    addFace( indexList );
    indexList[0] = 4;
    indexList[1] = 7;
    indexList[2] = 6;
    indexList[3] = 5;
    addFace( indexList );
    indexList[0] = 0;
    indexList[1] = 3;
    indexList[2] = 7;
    indexList[3] = 4;
    addFace( indexList );
    indexList[0] = 5;
    indexList[1] = 6;
    indexList[2] = 2;
    indexList[3] = 1;
    addFace( indexList );
    indexList[0] = 0;
    indexList[1] = 4;
    indexList[2] = 5;
    indexList[3] = 1;
    addFace( indexList );
    indexList[0] = 7;
    indexList[1] = 3;
    indexList[2] = 2;
    indexList[3] = 6;
    addFace( indexList );
    //qDebug("Box::initTriangles() end");
}
