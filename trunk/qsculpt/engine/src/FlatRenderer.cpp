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
#include "FlatRenderer.h"
#include "IObject3D.h"
#include <QtOpenGL>
#include "BufferObject.h"

VertexBuffer g_vbo;
VertexBuffer g_nbo;
IndexBuffer g_ibo;

bool g_enableVBO = false;

FlatRenderer::FlatRenderer()
{
	qDebug() << "FlatRenderer constructor";
}

FlatRenderer::~FlatRenderer()
{
	qDebug() << "FlatRenderer destructor";
}

void FlatRenderer::renderObject(const IObject3D* mesh)
{
	renderVbo(mesh);
	//renderImmediate(mesh);
}

void FlatRenderer::renderImmediate(const IObject3D* mesh)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	// Set the depth function to the correct value
    glDepthFunc(GL_LESS);

  	// Store the transformation matrix
  	glPushMatrix();
  	float x = 0.0f, y = 0.0f, z = 0.0f;
  	mesh->getPosition(&x, &y, &z);
   	glTranslatef(x, y, z);
	
   	QColor color;
	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();
    for ( int i = 0; i < size; i++)
    {
        glBegin(GL_POLYGON);
        const Face& f = faceList[i];
        for (int j = 0; j < f.point.size(); ++j)
        {
            // Change color of the object if it is selected;
        	color = Qt::white; //mesh->getPointList().at(f.normal[j]).color;
        	if (mesh->isSelected())
        	{
        		glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
        	}
        	else
        	{
        		glColor3d(color.redF(), color.greenF(), color.blueF());
        	}
            glNormal3fv(mesh->getNormalList().at(f.point[j]).getPoint());
            glVertex3fv(mesh->getPointList().at(f.point[j]).getPoint());
        }
        glEnd();
    }
    // Restore the transformation matrix
    glPopMatrix();
}

void FlatRenderer::renderVbo(const IObject3D* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	if (g_vbo.getBufferID() == 0)
		g_vbo.create();
	if (g_nbo.getBufferID() == 0)
		g_nbo.create();
//	if (g_ibo.getBufferID() == 0)
//		g_ibo.create();
	
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
	
	if (obj->hasChanged() && numFloats > 0)
	{
		qDebug() << "FlatRendererVBO::renderObject Start time:" << QDateTime::currentDateTime();
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

		g_vbo.setBufferData((GLvoid*)vtxData, numFloats*sizeof(GL_FLOAT));
		g_nbo.setBufferData((GLvoid*)nmlData, numFloats*sizeof(GL_FLOAT));
		
		delete [] vtxData;
		delete [] nmlData;
		qDebug() << "FlatRendererVBO::renderObject End time:" << QDateTime::currentDateTime();
		
		obj->setChanged(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo.getBufferID());
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, g_nbo.getBufferID());
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