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
			// TODO: clean up this
			float tmp[3];
			tmp[0] = mesh->getNormalList().at(f.point[j]).x();
			tmp[1] = mesh->getNormalList().at(f.point[j]).y();
			tmp[2] = mesh->getNormalList().at(f.point[j]).z();
            glNormal3fv(tmp);
			tmp[0] = mesh->getPointList().at(f.point[j]).x();
			tmp[1] = mesh->getPointList().at(f.point[j]).y();
			tmp[2] = mesh->getPointList().at(f.point[j]).z();
            glVertex3fv(tmp);
        }
        glEnd();
    }
}

void FlatRenderer::renderVbo(const IObject3D* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	
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
	glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), NULL);
	
	QColor color = Qt::white;
	if (mesh->isSelected())
		glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
	else
		glColor3d(color.redF(), color.greenF(), color.blueF());

	//qDebug() << "Draw mesh";
	glDrawArrays(GL_QUADS, 0, obj->getFaceList().size()*4);
	
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

VertexBuffer* FlatRenderer::getVBO(IObject3D* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

void FlatRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo)
{
	//qDebug() << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
	if (mesh == NULL || vbo->getBufferID() == 0)
		return;
	
	int numFaces = mesh->getFaceList().size();
	if (numFaces == 0)
		return;
	
	int numVertices = numFaces*4;	
	FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
	
	float tmp[3];
	int vertexIndex;
	for (int i = 0; i < numFaces; ++i)
	{
		for (int j = 0; j<4; ++j)
		{
			vertexIndex = mesh->getFaceList().at(i).point[j];
			// TODO: clean up this
			tmp[0] = mesh->getPointList().at(vertexIndex).x();
			tmp[1] = mesh->getPointList().at(vertexIndex).y();
			tmp[2] = mesh->getPointList().at(vertexIndex).z();
			memcpy(vtxData[i*4 + j].v, tmp, SIZE_OF_VERTEX);
			tmp[0] = mesh->getNormalList().at(vertexIndex).x();
			tmp[1] = mesh->getNormalList().at(vertexIndex).y();
			tmp[2] = mesh->getNormalList().at(vertexIndex).z();
			memcpy(vtxData[i*4 + j].n, tmp, SIZE_OF_NORMAL);
		}
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numVertices*sizeof(FlatVtxStruct));
	
	delete [] vtxData;
	
	//qDebug() << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}
