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
#include "ISurface.h"
#include <QtOpenGL>
#include <QMap>
#include "BOManager.h"


#define BO_POOL_NAME "FlatRendererPool"

#define SIZE_OF_VERTEX (3*sizeof(GLfloat))
#define SIZE_OF_NORMAL (3*sizeof(GLfloat))
#define SIZE_OF_COLOR (4*sizeof(GLubyte))
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

bool g_enableVBO = false;


typedef struct tagFlatVtxStruct 
	{
		GLfloat v[3];
		GLfloat n[3];
		GLubyte color[4];
	} FlatVtxStruct;

FlatRenderer::FlatRenderer()
{
	qDebug() << "FlatRenderer constructor";
}

FlatRenderer::~FlatRenderer()
{
	qDebug() << "FlatRenderer destructor";
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void FlatRenderer::renderObject(const ISurface* mesh)
{
	renderVbo(mesh);
	//renderImmediate(mesh);
}

void FlatRenderer::renderImmediate(const ISurface* mesh)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	// Set the depth function to the correct value
    glDepthFunc(GL_LESS);
    
    QColor color = Qt::white; //mesh->getPointList().at(f.normal[j]).color;
    if (mesh->isSelected())
    {
        glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
    }
    else
    {
        glColor3d(color.redF(), color.greenF(), color.blueF());
    }
    

    int fcounter = 0;
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        const Face& f = it.next();
        qDebug() << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        glBegin(GL_POLYGON);
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            qDebug() << "Vertex:" << toString(v.position());
            glNormal3fv(v.normal().data());
            glVertex3fv(v.position().data());
        }
        glEnd();
    }
}

void FlatRenderer::renderVbo(const ISurface* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	ISurface* obj = const_cast<ISurface*>(mesh);
	
	VertexBuffer* vbo = getVBO(obj);
	if (vbo == NULL || vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
		return;
	}
	// Set the depth function to the correct value
	glDepthFunc(GL_LESS);
	
	if ( vbo->needUpdate())
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
	glNormalPointer(GL_FLOAT, sizeof(FlatVtxStruct), BUFFER_OFFSET(SIZE_OF_VERTEX));
	glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), 0);
	
	QColor color = Qt::white;
	if (mesh->isSelected())
		glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
	else
		glColor3d(color.redF(), color.greenF(), color.blueF());

	//qDebug() << "Draw mesh";
	glDrawArrays(GL_QUADS, 0, obj->getNumFaces()*4);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
//	glDepthFunc(GL_ALWAYS);
//	glPointSize(2.0);
//	glBegin(GL_POINTS);
//	QVector<int> selectedVertices = mesh->getSelectedPoints();
//	for (int i = 0; i < selectedVertices.size(); ++i)
//	{
//		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//		glVertex3fv(mesh->getPointList().at(selectedVertices[i]).getPoint());
//	}
//	glEnd();
//	glDepthFunc(GL_LESS);
	
	//qDebug() << "Mesh rendered";
}

VertexBuffer* FlatRenderer::getVBO(ISurface* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

void FlatRenderer::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
	//qDebug() << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
	if (mesh == NULL || vbo->getBufferID() == 0)
		return;
	
	int numFaces = mesh->getNumFaces();
	if (numFaces == 0)
		return;
	
	int numVertices = numFaces*4;
	FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
	
    int fcounter = 0;
    int offset = 0;
	Iterator<Face> it = mesh->constFaceIterator();
	while(it.hasNext()) {
		const Face& f = it.next();
        qDebug() << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            qDebug() << "Vertex:" << toString(v.position());
            vtxData[offset].v[0] = v.position().x();
            vtxData[offset].v[1] = v.position().y();
            vtxData[offset].v[2] = v.position().z();
            
            vtxData[offset].n[0] = v.normal().x();
            vtxData[offset].n[1] = v.normal().y();
            vtxData[offset].n[2] = v.normal().z();
            
            vtxData[offset].color[0] = 0.8;
            vtxData[offset].color[1] = 0.8;
            vtxData[offset].color[2] = 0.8;
            vtxData[offset].color[3] = 1.0;
            offset++;
        }        
	}
	vbo->setBufferData((GLvoid*)vtxData, numVertices*sizeof(FlatVtxStruct));
	
	delete [] vtxData;
	
	//qDebug() << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}
