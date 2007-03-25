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
#include "stdafx.h"
#include "sphere.h"
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


void Sphere::draw()
{
    Object3D::draw();
}

void Sphere::initPoints()
{
/*    
	double x, y, z;
    
    double hw = m_width / 2;
    double hh = m_height / 2;
    double hd = m_depth / 2;
	
    m_pointList.append(Point( 0, 0, hd));
    m_pointList.append(Point( 0, 0,-hd));
    
    m_pointList.append(Point(-hw,-hh, 0.0));
    m_pointList.append(Point( hw,-hh, 0.0));
    m_pointList.append(Point( hw, hh, 0.0));
    m_pointList.append(Point(-hw, hh, 0.0));
    
    Point3D normal;
    int newTriangleIndex = -1;
    m_triangleList.append(Object3D::Triangle(0, 2, 3));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(0, 3, 4));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(0, 4, 5));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(0, 5, 2));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(1, 3, 2));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(1, 4, 3));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(1, 5, 4));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    m_triangleList.append(Object3D::Triangle(1, 2, 5));
    normal = computeTriangleNormal( m_triangleList.size() -1);
    m_normalList.append(normal);
    newTriangleIndex = m_triangleList.size() - 1;
    m_triangleList[newTriangleIndex].normal[0] = m_normalList.size() - 1;
    m_triangleList[newTriangleIndex].normal[1] = m_triangleList[newTriangleIndex].normal[0];
    m_triangleList[newTriangleIndex].normal[2] = m_triangleList[newTriangleIndex].normal[0];
    
    subdivide();
    */
}

