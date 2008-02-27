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
	qDebug() << "SmoothRenderer constructor";
}

SmoothRenderer::~SmoothRenderer()
{
	qDebug() << "SmoothRenderer destructor";
	if (m_vertexBuffer.getBufferID() != 0)
		m_vertexBuffer.destroy();
	if (m_normalBuffer.getBufferID() != 0)
		m_normalBuffer.destroy();
}

void SmoothRenderer::renderObject(const IObject3D* mesh)
{
	renderVbo(mesh);
}

void SmoothRenderer::renderImmediate(const IObject3D* mesh)
{
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

void SmoothRenderer::renderVbo(const IObject3D* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	bool buildVBO = false;
	
	if (m_vertexBuffer.getBufferID() == 0)
	{
		m_vertexBuffer.create();
		buildVBO = true;
	}
	if (m_normalBuffer.getBufferID() == 0)
	{
		m_normalBuffer.create();
		buildVBO = true;
	}
	
	// Set the depth function to the correct value
    glDepthFunc(GL_LESS);
	
  	// Store the transformation matrix
  	glPushMatrix();
  	float x = 0.0f, y = 0.0f, z = 0.0f;
  	mesh->getPosition(&x, &y, &z);
   	glTranslatef(x, y, z);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	int numFaces = obj->getFaceList().size();
	int numFloats = numFaces*4*3;	// num floats = Faces * Num of point by face
	//				* Num elements of point
	
	if ((obj->hasChanged() || buildVBO ) && numFloats > 0)
	{
		qDebug() << "SmoothRenderer::renderObject Start time:" << QDateTime::currentDateTime();
		qDebug() << "Num Vertices=" << obj->getPointList().size();
		qDebug() << "Num Normals=" << obj->getNormalList().size();
		GLfloat* vtxData = new GLfloat[numFloats];
		GLfloat* nmlData = new GLfloat[numFloats];
		
		int vertexIndex;
		for (int i = 0; i < numFaces; ++i)
		{
			for (int j = 0; j<4; ++j)
			{
				vertexIndex = obj->getFaceList().at(i).point[j];
				vtxData[(i*12) + (j*3)] = obj->getPointList().at(vertexIndex).getX();
				vtxData[(i*12) + (j*3) + 1] = obj->getPointList().at(vertexIndex).getY();
				vtxData[(i*12) + (j*3) + 2] = obj->getPointList().at(vertexIndex).getZ();
				
				nmlData[(i*12) + (j*3)] = obj->getNormalList().at(vertexIndex).getX();
				nmlData[(i*12) + (j*3) + 1] = obj->getNormalList().at(vertexIndex).getY();
				nmlData[(i*12) + (j*3) + 2] = obj->getNormalList().at(vertexIndex).getZ();
			}
		}
		
		m_vertexBuffer.setBufferData((GLvoid*)vtxData, numFloats*sizeof(GL_FLOAT));
		m_normalBuffer.setBufferData((GLvoid*)nmlData, numFloats*sizeof(GL_FLOAT));
		
		delete [] vtxData;
		delete [] nmlData;
		qDebug() << "SmoothRenderer::renderObject End time:" << QDateTime::currentDateTime();
		
		obj->setChanged(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.getBufferID());
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer.getBufferID());
	glNormalPointer(GL_FLOAT, 0, NULL);
	
	QColor color;
	color = Qt::white; //mesh->getPointList().at(f.normal[j]).color;
	if (mesh->isSelected())
	{
		glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
	}
	else
	{
		glColor3d(color.redF(), color.greenF(), color.blueF());
	}
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo.getBufferID());
	glDrawArrays(GL_QUADS, 0, obj->getFaceList().size()*4);
	//glDrawElements(GL_QUADS, obj->getFaceList().size()*4, GL_UNSIGNED_INT, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glPopMatrix();
}