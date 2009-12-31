#include "StdAfx.h"
#include "WireframeRenderer.h"
#include <QtOpenGL>
#include "IObject3D.h"
#include <QPolygon>
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
	glColor3f(0.0f, 0.0f, 1.0f);
    for ( int i = 0; i < size; i++)
    {
        const Face& f = faceList[i];
        int size = f.point.size();
        for (int j = 0; j < size; ++j)
        {
            glVertex3f(mesh->getPointList().at(f.point[j]).x(),
					   mesh->getPointList().at(f.point[j]).y(),
					   mesh->getPointList().at(f.point[j]).z());
            glVertex3f(mesh->getPointList().at(f.point[(j + 1) % size]).x(),
					   mesh->getPointList().at(f.point[(j + 1) % size]).y(),
					   mesh->getPointList().at(f.point[(j + 1) % size]).z()
					   );
        }
    }
    glEnd();
}