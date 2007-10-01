#include "stdafx.h"
#include "flatrenderer.h"
#include "iobject3d.h"
#include <QtOpenGL>

FlatRenderer::FlatRenderer()
{
	//qDebug() << "FlatRenderer constructor";
}

FlatRenderer::~FlatRenderer()
{
	//qDebug() << "FlatRenderer destructor";
}

void FlatRenderer::renderObject(const IObject3D* mesh)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    // Change color of the object if it is selected;
	if (mesh->isSelected())
    	glColor3d(0.0, 1.0, 0.0);
    else
    	//qglColor(mesh->getColor());
    	glColor3d(0.8, 0.8, 0.8);
    glDepthFunc(GL_LESS);

  	// Store the transformation matrix
  	glPushMatrix();
  	float x = 0.0f, y = 0.0f, z = 0.0f;
  	mesh->getPosition(&x, &y, &z);
   	glTranslatef(x, y, z);

	const FaceContainer& faceList = mesh->getFaceList();
    int size = faceList.size();
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
    // Restore the transformation matrix
    glPopMatrix();
}