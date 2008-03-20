#include "StdAfx.h"
#include "PointRenderer.h"
#include <QtOpenGL>
#include "IObject3D.h"

PointRenderer::PointRenderer()
{
	qDebug() << "PointRenderer constructor";
}

PointRenderer::~PointRenderer()
{
	qDebug() << "PointRenderer destructor";
	if (m_vertexBuffer.getBufferID() != 0)
		m_vertexBuffer.destroy();
	if (m_colorBuffer.getBufferID() != 0)
		m_colorBuffer.destroy();
}

void PointRenderer::renderObject(const IObject3D* mesh)
{
	renderVbo(mesh);
}

void PointRenderer::renderImmediate(const IObject3D* mesh)
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

void PointRenderer::renderVbo(const IObject3D* mesh)
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
	if (m_colorBuffer.getBufferID() == 0)
	{
		m_colorBuffer.create();
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
	glEnableClientState(GL_COLOR_ARRAY);
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	int numVertices = obj->getPointList().size();
	int numFloats = numVertices*3;
	
	if ((obj->hasChanged() || buildVBO ) && numVertices > 0)
	{
		qDebug() << "PointRenderer::renderObject Start time:" << QDateTime::currentDateTime();
		qDebug() << "Num Vertices=" << numVertices;
		GLfloat* vtxData = new GLfloat[numFloats];
		GLubyte* colorData = new GLubyte[numFloats];
		
		GLuint color = 0xFFFFFF;
		for (int i = 0; i < numVertices; ++i)
		{
			vtxData[(i*3)] = obj->getPointList().at(i).getX();
			vtxData[(i*3) + 1] = obj->getPointList().at(i).getY();
			vtxData[(i*3) + 2] = obj->getPointList().at(i).getZ();
			
			colorData[(i*3)] = (color & 0xFF);
			colorData[(i*3) + 1] = ((color >> 8) & 0xFF);
			colorData[(i*3) + 2] = ((color >> 16) & 0xFF);
			
			color--;
		}
		
		m_vertexBuffer.setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));
		m_colorBuffer.setBufferData((GLvoid*)colorData, numFloats*sizeof(GLubyte));
		
		delete [] vtxData;
		qDebug() << "PointRenderer::renderObject End time:" << QDateTime::currentDateTime();
		
		obj->setChanged(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.getBufferID());
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer.getBufferID());
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, NULL);
	
	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glPopMatrix();
}