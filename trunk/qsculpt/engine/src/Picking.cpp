/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores   *
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
#include <QtOpenGL>
#include "Picking.h"

#include "IObject3D.h";

inline bool printGlError()
{
	bool result = false;
	while( (GLenum error = glGetError()) != GL_NO_ERROR )
	{
		result = true;
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}

PickingRenderer::PickingRenderer()
{
}

PickingRenderer::~PickingRenderer()
{
	qDebug() << "PointRenderer destructor";
	foreach(VertexBuffer* vbo, m_vboContainer)
	{
		vbo->destroy();
		delete vbo;
	}
	m_vboContainer.clear();
}

char* intToHexStr(unsigned int d)
{
	const char* symbols = "0123456789ABCDEF";
	char* output = new char[34];
	memset(output, 0, 34*sizeof(char));
	
	unsigned int digit = 0, c = d;
	int index = 0;
	do
	{
		digit = c % 16;
		c = c / 16;
		output[index] = symbols[digit];
		index++;
	}while(c);
	
	// get the length of the string
	int len = 0;
	char* ptr = output;
	while(*ptr)
	{ len++; ptr++;}
	
	// Reverse the order of the string
	for (int i =0; i < len/2; ++i)
	{
		char tmp = output[i];
		output[i] = output[len - i - 1];
		output[len - i - 1] = tmp;
	}
	return output;
}

PickingRenderer::ObjectList PickingRenderer::getSelectedObjects(const ObjectList& list, GLint x, GLint y)
{
	ObjectList l;

	unsigned int objId = 1;
	GLuint data = 0;
	foreach(IObject3D* mesh, list)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderVbo(mesh, objId);
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&data);

		if (data == objId)
			l.append(mesh);
		
		objId++;
		
		if (printGlError()) break;
	}

	return l;
}

PickingRenderer::ObjectVerticesMap PickingRenderer::getSelectedVertices(const ObjectList& list, GLfloat x, GLfloat y)
{
	ObjectVerticesMap map;
	return map;
}

PickingRenderer::ObjectFacesMap PickingRenderer::getSelectedFaces(const ObjectList& list, GLfloat x, GLfloat y)
{
	ObjectFacesMap map;
	return map;
}

void PickingRenderer::renderImmediate(const IObject3D* mesh, unsigned int objID)
{
	mesh->lock();
    int size = mesh->getPointList().size();
	
    glBegin(GL_POINTS);
    for ( int i = 0; i < size; i++)
    {
        glVertex3fv(mesh->getPointList().at(i).getPoint());
    }
    glEnd();
	mesh->unlock();
}

void PickingRenderer::renderVbo(const IObject3D* mesh, unsigned int objID)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	VertexBuffer *vbo = getVBO(obj);	
	if (vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh, objID);
		return;
	}
	
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
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
	glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);
	
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT|GL_LIGHTING_BIT|GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glPointSize(1.0f);
	glColor4ub( objID & 0xff,(objID>>8) & 0xff, (objID>>16) & 0xff, (objID>>24) & 0xff);
	glDrawArrays(GL_QUADS, 0, obj->getFaceList().size()*4);
	
	printGlError();
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopAttrib();
	
	glPopMatrix();
}

VertexBuffer* PickingRenderer::getVBO(IObject3D* mesh)
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

void PickingRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo)
{
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
}
