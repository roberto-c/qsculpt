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
#include "BOManager.h"
#include <QMap>

typedef QMap<IObject3D*, VertexBuffer* > BOContainer;

BOContainer g_vboContainer;

bool g_enableVBO = false;

FlatRenderer::FlatRenderer()
{
	qDebug() << "FlatRenderer constructor";
}

FlatRenderer::~FlatRenderer()
{
	qDebug() << "FlatRenderer destructor";
	foreach(VertexBuffer* vbo, g_vboContainer)
	{
		vbo->destroy();
		delete vbo;
	}
	g_vboContainer.clear();
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
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	
	VertexBuffer* vbo = getVBO(obj);
	qDebug() << "VBO id= " << vbo->getBufferID();
	// Set the depth function to the correct value
    glDepthFunc(GL_LESS);
	
  	// Store the transformation matrix
  	glPushMatrix();
  	float x = 0.0f, y = 0.0f, z = 0.0f;
  	mesh->getPosition(&x, &y, &z);
   	glTranslatef(x, y, z);
	
	bool vboNeedsRefresh = vbo->needUpdate() || obj->hasChanged();
	if (vboNeedsRefresh)
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
		obj->setChanged(false);
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
	glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), NULL);
	//glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
	
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

	glDrawArrays(GL_POINTS, 0, 4/*obj->getFaceList().size()*4*/);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glPopMatrix();
}

VertexBuffer* FlatRenderer::getVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	if (g_vboContainer.contains(mesh))
	{
		vbo = g_vboContainer[mesh];
	}
	else
	{
		vbo = new VertexBuffer;
		vbo->create();
		g_vboContainer[mesh] = vbo;
	}
	return vbo;
}

void FlatRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo)
{
	qDebug() << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
	qDebug() << "Num Vertices=" << mesh->getPointList().size();
	qDebug() << "Num Normals=" << mesh->getNormalList().size();
	int numFaces = mesh->getFaceList().size();
	if (numFaces == 0)
		return;
	
	int numFloats = numFaces*4*6;	// num floats = Faces * Num of point by face
									//				* Num elements of point
	GLfloat* vtxData = new GLfloat[numFloats]; // Make room for vertices and normals data
	
	int vertexIndex;
	for (int i = 0; i < numFaces; ++i)
	{
		for (int j = 0; j<4; ++j)
		{
			vertexIndex = mesh->getFaceList().at(i).point[j];
			vtxData[(i*24) + (j*6)] = mesh->getPointList().at(vertexIndex).getX();
			vtxData[(i*24) + (j*6) + 1] = mesh->getPointList().at(vertexIndex).getY();
			vtxData[(i*24) + (j*6) + 2] = mesh->getPointList().at(vertexIndex).getZ();
			
			vtxData[(i*24) + (j*6) + 3] = mesh->getNormalList().at(vertexIndex).getX();
			vtxData[(i*24) + (j*6) + 4] = mesh->getNormalList().at(vertexIndex).getY();
			vtxData[(i*24) + (j*6) + 5] = mesh->getNormalList().at(vertexIndex).getZ();
		}
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));
	
	delete [] vtxData;
	qDebug() << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}
