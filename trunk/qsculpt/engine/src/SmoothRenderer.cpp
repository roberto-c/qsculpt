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
#include "SmoothRenderer.h"
#include "IObject3D.h"
#include <QtOpenGL>

SmoothRenderer::SmoothRenderer()
{
	//qDebug() << "SmoothRenderer constructor";
}

SmoothRenderer::~SmoothRenderer()
{
	//qDebug() << "SmoothRenderer destructor";
}

void SmoothRenderer::renderObject(const IObject3D* mesh)
{
    //TODO: Bound checking missing
	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();

    if (mesh->isSelected())
    	glColor3d(0.0, 1.0, 0.0);
    else
    	glColor3d(0.8, 0.8, 0.8);

    for ( int i = 0; i < size; i++)
    {
//        if (faceList[i].isMarked)
//            glColor3f(1.0, 0.0, 0.0);
//        else
//            glColor3f(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        const Face& f = faceList[i];
        for (int j = 0; j < f.point.size(); ++j)
        {
            glNormal3fv(mesh->getNormalList().at(f.point[j]).getPoint());
            glVertex3fv(mesh->getPointList().at(f.point[j]).getPoint());
        }
        glEnd();
    }
}