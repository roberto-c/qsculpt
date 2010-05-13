/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores                      *
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
#include "StdAfx.h"
#include "PointRenderer.h"
#include <QtOpenGL>
#include "IObject3D.h"
#include <QMap>
#include "BOManager.h"

#define BO_POOL_NAME "PointRendererPool"

PointRenderer::PointRenderer()
{
	qDebug() << "PointRenderer constructor";
}

PointRenderer::~PointRenderer()
{
	qDebug() << "PointRenderer destructor";
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void PointRenderer::renderObject(const IObject3D* mesh)
{
	renderVbo(mesh);
}

void PointRenderer::renderImmediate(const IObject3D* mesh)
{
	mesh->lock();
    int numVertices = mesh->getNumVertices();
	if (numVertices == 0)
		return;

    glBegin(GL_POINTS);
	Iterator<Vertex> it = mesh->constVertexIterator();
	while(it.hasNext()) {
		const Vertex& v = it.next();
        glVertex3fv(v.position().data());
	}
    glEnd();
	mesh->unlock();
}

void PointRenderer::renderVbo(const IObject3D* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	VertexBuffer *vbo = getVBO(obj);	
	if (vbo == NULL || vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
		return;
	}
	
	if (vbo->needUpdate())
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
	glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);
	glColorPointer(3, GL_FLOAT, 6*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
	
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	
	glPointSize(3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_POINTS, 0, obj->getNumVertices());
	
//	glDepthFunc(GL_EQUAL);
//	glPointSize(1.5f);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	
	glPopAttrib();
}

VertexBuffer* PointRenderer::getVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

void PointRenderer::fillVertexBuffer(const IObject3D* mesh, VertexBuffer* vbo)
{
	int numVertices = mesh->getNumVertices();
	if (numVertices == 0)
		return;
	
	int numFloats = numVertices*6;
	GLfloat* vtxData = new GLfloat[numFloats];
	
	Iterator<Vertex> it = mesh->constVertexIterator();
	int offset = 0;
	while(it.hasNext()) {
		const Vertex& v = it.next();
		vtxData[offset] = v.position().x();
		offset++;
		vtxData[offset] = v.position().y();
		offset++;
		vtxData[offset] = v.position().z();
		offset++;
		
		vtxData[offset] = 0.85f;
		offset++;
		vtxData[offset] = 0.85f;
		offset++;
		vtxData[offset] = 0.85f;
		offset++;
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));
	
	delete [] vtxData;
}
