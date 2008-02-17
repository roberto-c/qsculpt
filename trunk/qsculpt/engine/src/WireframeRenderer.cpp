#include "StdAfx.h"
#include "WireframeRenderer.h"
#include <QtOpenGL>
#include "IObject3D.h"

WireframeRenderer::WireframeRenderer()
{
	qDebug() << "WireframeRenderer constructor";
}

WireframeRenderer::~WireframeRenderer()
{
	qDebug() << "WireframeRenderer destructor";
}

void WireframeRenderer::renderObject(const IObject3D* mesh)
{
	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();

    glBegin(GL_LINES);
    for ( int i = 0; i < size; i++)
    {
        const Face& f = faceList[i];
        int size = f.point.size();
        for (int j = 0; j < size; ++j)
        {
            glVertex3fv(mesh->getPointList().at(f.point[j]).getPoint());
            glVertex3fv(mesh->getPointList().at(f.point[(j + 1) % size]).getPoint());
        }
    }
    glEnd();
}