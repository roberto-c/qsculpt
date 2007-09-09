#include "stdafx.h"
#include "smoothrenderer.h"
#include "iobject3d.h"
#include <QtOpenGL>

SmoothRenderer::SmoothRenderer()
{
	qDebug() << "SmoothRenderer constructor";
}

SmoothRenderer::~SmoothRenderer()
{
	qDebug() << "SmoothRenderer destructor";
}

void SmoothRenderer::renderObject(const IObject3D* mesh)
{
    //TODO: Bound checking missing
	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();

    if (mesh->isSelected())
    	glColor3d(0.0, 1.0, 0.0);
    else
    	glColor3d(0.8, 0.8, 0.8);

    for ( int i = 0; i < size; i++)
    {
//        if (faceList[i].isMarked)
//            glColor3f(1.0, 0.0, 0.0);
//        else
//            glColor3f(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        const Face& f = faceList[i];
        for (int j = 0; j < f.point.size(); ++j)
        {
            glNormal3fv(mesh->getNormalList().at(f.normal[j]).getPoint());
            glVertex3fv(mesh->getPointList().at(f.point[j]).vertex.getPoint());
        }
        glEnd();
    }
}