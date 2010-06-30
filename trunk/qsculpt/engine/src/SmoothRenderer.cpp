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
#include "SmoothRenderer.h"
#include "ISurface.h"
#include <QtOpenGL>
#include "BOManager.h"

#define BO_POOL_NAME "SmoothRendererPool"

SmoothRenderer::SmoothRenderer()
{
	qDebug() << "SmoothRenderer constructor";
}

SmoothRenderer::~SmoothRenderer()
{
	qDebug() << "SmoothRenderer destructor";
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void SmoothRenderer::renderObject(const ISurface* mesh)
{
	renderVbo(mesh);
}

void SmoothRenderer::renderImmediate(const ISurface* mesh)
{	
	if (mesh->isSelected())
		glColor3d(0.0, 1.0, 0.0);
	else
		glColor3d(0.8, 0.8, 0.8);

    
    Iterator<Face> it = mesh->constFaceIterator();
	while(it.hasNext()) {
		const Face& f = it.next();
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        glBegin(GL_POLYGON);
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            glNormal3fv(v.normal().data());
            glVertex3fv(v.position().data());
            // qDebug() << "Vertex:" << toString(v.position());
        }
        glEnd();
	}
    
}

void SmoothRenderer::renderVbo(const ISurface* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	ISurface* obj = const_cast<ISurface*>(mesh);
	VertexBuffer* vbo= getVBO(obj);
	if (vbo == NULL || vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
		return;
	}
	
	// Set the depth function to the correct value
	glDepthFunc(GL_LESS);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	if (vbo->needUpdate())
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
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
	
	glDrawArrays(GL_QUADS, 0, obj->getNumFaces()*4);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

VertexBuffer* SmoothRenderer::getVBO(ISurface* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;	
}

void SmoothRenderer::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
	qDebug() << "SmoothRenderer::renderObject Start time:" << QDateTime::currentDateTime();
	int numFaces = mesh->getNumFaces();
	int numFloats = numFaces*4*6;
	GLfloat* vtxData = new GLfloat[numFloats];

    int offset = 0;
	Iterator<Face> it = mesh->constFaceIterator();
	while(it.hasNext()) {
		const Face& f = it.next();
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            // qDebug() << "Vertex:" << toString(v.position());
            vtxData[offset++] = v.position().x();
            vtxData[offset++] = v.position().y();
            vtxData[offset++] = v.position().z();
            
            vtxData[offset++] = v.normal().x();
            vtxData[offset++] = v.normal().y();
            vtxData[offset++] = v.normal().z();
        }        
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));
	
	delete [] vtxData;
	qDebug() << "SmoothRenderer::renderObject End time:" << QDateTime::currentDateTime();
}