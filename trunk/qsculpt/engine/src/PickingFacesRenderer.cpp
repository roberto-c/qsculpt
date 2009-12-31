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
#include "PickingFacesRenderer.h"

#include "IObject3D.h"
#include "BOManager.h"

#define BO_POOL_NAME "PickingVertexRendererPool"
#define BO_FLAT_POOL_NAME "PickingVertexRendererPool_Flat"

typedef struct tagVtxStruct 
{
	GLfloat v[3];
	GLubyte color[4];
} VtxStruct;

typedef struct tagFlatVtxStruct 
{
	GLfloat v[3];
	GLfloat n[3];
	GLubyte color[4];
} FlatVtxStruct;

#define SIZE_OF_VERTEX (3*sizeof(GLfloat))
#define SIZE_OF_NORMAL (3*sizeof(GLfloat))
#define SIZE_OF_COLOR (4*sizeof(GLubyte))
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

PickingFacesRenderer::PickingFacesRenderer()
{
}
PickingFacesRenderer::~PickingFacesRenderer()
{
	//BOManager::getInstance()->destroyPool(BO_POOL_NAME);
	BOManager::getInstance()->destroyPool(BO_FLAT_POOL_NAME);
}
		
void PickingFacesRenderer::renderObject(const IObject3D* mesh, GLuint objId)
{
	renderVbo(mesh, objId);
}
		
void PickingFacesRenderer::renderVbo(const IObject3D* mesh, unsigned int objId)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	//VertexBuffer *vbo = getVBO(obj);
	VertexBuffer *flatVbo = getFlatVBO(obj);
	if (//vbo == NULL || vbo->getBufferID() == 0 ||
		 flatVbo == NULL || flatVbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh, objId);
		return;
	}
	
	if (/*vbo->needUpdate() || */flatVbo->needUpdate())
	{
		//fillVertexBuffer(obj, vbo, objId);
		fillFlatVertexBuffer(obj, flatVbo, objId);
		//vbo->setNeedUpdate(false);
		flatVbo->setNeedUpdate(false);
	}
	
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT);
	
	glBindBuffer(GL_ARRAY_BUFFER, flatVbo->getBufferID());
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(FlatVtxStruct),
				   BUFFER_OFFSET(SIZE_OF_VERTEX + SIZE_OF_NORMAL));
	glNormalPointer(GL_FLOAT, sizeof(FlatVtxStruct), BUFFER_OFFSET(SIZE_OF_VERTEX));
	glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), NULL);
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	//glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_QUADS, 0, obj->getFaceList().size()*4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
//	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
//	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VtxStruct), BUFFER_OFFSET(SIZE_OF_VERTEX));
//	glVertexPointer(3, GL_FLOAT, sizeof(VtxStruct), NULL);
//	
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
//	
//	glPointSize(1.0f);
//	//glColor3f(1.0f, 1.0f, 1.0f);
//	glDepthFunc(GL_LEQUAL);
//	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
//	
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
	
	glPopAttrib();
}

void PickingFacesRenderer::renderImmediate(const IObject3D* mesh, unsigned int objID)
{
	mesh->lock();
    int size = mesh->getPointList().size();
	
    glBegin(GL_POINTS);
    for ( int i = 0; i < size; i++)
    {
        glVertex3f(mesh->getPointList().at(i).x(),
				   mesh->getPointList().at(i).y(),
				   mesh->getPointList().at(i).z());
    }
    glEnd();
	mesh->unlock();
}
		
VertexBuffer* PickingFacesRenderer::getVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

VertexBuffer* PickingFacesRenderer::getFlatVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_FLAT_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_FLAT_POOL_NAME, mesh);
	}
	return vbo;
}
		
void PickingFacesRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo, GLuint objId)
{
	int numVertices = mesh->getPointList().size();
	if (numVertices == 0)
		return;
	
	VtxStruct* vtxData = new VtxStruct[numVertices];
	
	for (int i = 0; i < numVertices; ++i)
	{
		vtxData[i].v[0] = mesh->getPointList().at(i).x();
		vtxData[i].v[1] = mesh->getPointList().at(i).y();
		vtxData[i].v[2] = mesh->getPointList().at(i).z();
		//memcpy(vtxData[i].v, mesh->getPointList().at(i).getPoint(), SIZE_OF_VERTEX);
		memcpy((void*)vtxData[i].color, (const GLubyte*)&objId, sizeof(objId));
		objId++;
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numVertices*sizeof(VtxStruct));
	
	delete [] vtxData;
}

void PickingFacesRenderer::fillFlatVertexBuffer(IObject3D* mesh, VertexBuffer* vbo, GLuint objId)
{
	if (mesh == NULL || vbo->getBufferID() == 0)
		return;
	
	int numFaces = mesh->getFaceList().size();
	if (numFaces == 0)
		return;
	
	int numVertices = numFaces*4;	
	FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
	
	int vertexIndex;
	for (int i = 0; i < numFaces; ++i)
	{
		for (int j = 0; j<4; ++j)
		{
			vertexIndex = mesh->getFaceList().at(i).point[j];
			vtxData[i*4 + j].v[0] = mesh->getPointList().at(vertexIndex).x();
			vtxData[i*4 + j].v[1] = mesh->getPointList().at(vertexIndex).y();
			vtxData[i*4 + j].v[2] = mesh->getPointList().at(vertexIndex).z();
			//memcpy(vtxData[i*4 + j].v, mesh->getPointList().at(vertexIndex).getPoint(), SIZE_OF_VERTEX);
			
			vtxData[i*4 + j].n[0] = mesh->getNormalList().at(vertexIndex).x();
			vtxData[i*4 + j].n[1] = mesh->getNormalList().at(vertexIndex).y();
			vtxData[i*4 + j].n[2] = mesh->getNormalList().at(vertexIndex).z();
			//memcpy(vtxData[i*4 + j].n, mesh->getNormalList().at(vertexIndex).getPoint(), SIZE_OF_NORMAL);
			
			memcpy((void*)vtxData[i*4 + j].color, (const GLubyte*)&objId, sizeof(objId));
		}
		objId++;
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numVertices*sizeof(FlatVtxStruct));
	
	delete [] vtxData;
}
