#include "stdafx.h"
#include "pointrenderer.h"
#include <QtOpenGL>
#include "iobject3d.h"

PointRenderer::PointRenderer()
{
	//qDebug() << "PointRenderer constructor";
}

PointRenderer::~PointRenderer()
{
	//qDebug() << "PointRenderer destructor";
}

void PointRenderer::renderObject(const IObject3D* mesh)
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
