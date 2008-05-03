#include "StdAfx.h"
#include "PointRenderer.h"
#include <QtOpenGL>
#include "IObject3D.h"
#include <QMap>

PointRenderer::PointRenderer()
{
	qDebug() << "PointRenderer constructor";
}

PointRenderer::~PointRenderer()
{
	qDebug() << "PointRenderer destructor";
	foreach(VertexBuffer* vbo, m_vboContainer)
	{
		vbo->destroy();
		delete vbo;
	}
	m_vboContainer.clear();
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
	
	IObject3D* obj = const_cast<IObject3D*>(mesh);
	VertexBuffer *vbo = getVBO(obj);	
	if (vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
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
	glColorPointer(3, GL_FLOAT, 6*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
	
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	
	glPointSize(3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
	
//	glDepthFunc(GL_EQUAL);
//	glPointSize(1.5f);
//	glColor3f(1.0f, 1.0f, 1.0f);
//	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	
	glPopAttrib();
	
	glPopMatrix();
}

VertexBuffer* PointRenderer::getVBO(IObject3D* mesh)
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

void PointRenderer::fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo)
{
	int numVertices = mesh->getPointList().size();
	if (numVertices == 0)
		return;
	
	int numFloats = numVertices*6;
	GLfloat* vtxData = new GLfloat[numFloats];
	
	for (int i = 0; i < numVertices; ++i)
	{
		vtxData[(i*6)] = mesh->getPointList().at(i).getX();
		vtxData[(i*6) + 1] = mesh->getPointList().at(i).getY();
		vtxData[(i*6) + 2] = mesh->getPointList().at(i).getZ();
		
		vtxData[(i*6) + 3] = 0.85f;
		vtxData[(i*6) + 4] = 0.85f;
		vtxData[(i*6) + 5] = 0.85f;
	}
	
	vbo->setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));
	
	delete [] vtxData;
}
