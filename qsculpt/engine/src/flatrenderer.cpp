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

	// Set the depth function to the correct value
    glDepthFunc(GL_LESS);

  	// Store the transformation matrix
  	glPushMatrix();
  	float x = 0.0f, y = 0.0f, z = 0.0f;
  	mesh->getPosition(&x, &y, &z);
   	glTranslatef(x, y, z);

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
            glNormal3fv(mesh->getNormalList().at(f.normal[j]).getPoint());
            glVertex3fv(mesh->getPointList().at(f.point[j]).getPoint());
        }
        glEnd();
    }
    // Restore the transformation matrix
    glPopMatrix();
}