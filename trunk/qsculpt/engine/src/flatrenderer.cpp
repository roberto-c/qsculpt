#include "stdafx.h"
#include "flatrenderer.h"
#include "iobject3d.h"
#include <QtOpenGL>

FlatRenderer::FlatRenderer()
{
	qDebug() << "FlatRenderer constructor";
}

FlatRenderer::~FlatRenderer()
{
	qDebug() << "FlatRenderer destructor";
}

void FlatRenderer::renderObject(const IObject3D* mesh)
{
	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();

    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh->isSelected())
    	glColor3d(0.0, 1.0, 0.0);
    else
    	glColor3d(0.8, 0.8, 0.8);
    glDepthFunc(GL_LESS);

    for ( int i = 0; i < size; i++)
    {
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