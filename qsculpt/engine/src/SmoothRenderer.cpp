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
	foreach(VertexBuffer* vbo, m_vboContainer)
	{
		vbo->destroy();
		delete vbo;
	}
	m_vboContainer.clear();
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
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	VertexBuffer* vbo= getVBO(obj);
	if (vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
		return;
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
	
	bool vboNeedsRefresh = vbo->needUpdate() || obj->hasChanged();
	if (vboNeedsRefresh)
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
		obj->setChanged(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
	glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);
	glNormalPointer(GL_FLOAT, 6*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
	
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
	
	glDrawArrays(GL_QUADS, 0, obj->getFaceList().size()*4);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glPopMatrix();
}

VertexBuffer* SmoothRenderer::getVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	if (m_vboContainer.contains(mesh))
	{
		vbo = m_vboContainer[mesh];
	}
	else
	{
		vbo = new VertexBuffer;
		vbo->create();
		m_vboContainer[mesh] = vbo;
	}
	return vbo;	
}

void SmoothRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo)
{
	qDebug() << "SmoothRenderer::renderObject Start time:" << QDateTime::currentDateTime();
	int numFaces = mesh->getFaceList().size();
	int numFloats = numFaces*4*6;
	GLfloat* vtxData = new GLfloat[numFloats];
	
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
	qDebug() << "SmoothRenderer::renderObject End time:" << QDateTime::currentDateTime();
}
