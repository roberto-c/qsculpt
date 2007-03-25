/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#include "commandbase.h"
#include <QtOpenGL>
#include <QMouseEvent>
#include "configcontainer.h"
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "idocument.h"
#include "iobject3d.h"
#include "documentview.h"
#include "camera.h"

CommandBase::CommandBase(): ICommand(),
    m_configContainer(new ConfigContainer()),
    m_isActive(false),
    m_rotationRadius(0.0),
    m_currentCamera(NULL),
    m_intialCameraState(new Camera),
    m_panViewMode(false),
    m_rotateViewMode(false)
{
}


CommandBase::~CommandBase()
{
}


IConfigContainer& CommandBase::getConfig() const
{
    return *m_configContainer;
}

void CommandBase::activate(bool active)
{
    m_isActive = active;
}

bool CommandBase::isActive()
{
    return m_isActive;
}

void CommandBase::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
    
    GLdouble x, y, z;
    GLfloat wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
    glGetIntegerv(GL_VIEWPORT, m_viewPort);

	glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), m_viewPort[3] - e->y(), wz, m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

	m_initialWinPoint = Point3D(e->x(), m_viewPort[3] - e->y(), wz);
	m_currentWinPoint = m_initialWinPoint;
    m_intialPoint = Point3D(x, y, z);
    m_currentPoint = m_intialPoint;
	qDebug("wz: %f %s", wz, qPrintable(m_intialPoint.toString()));

    m_currentCamera = view->getViewCamera();
    *m_intialCameraState = *m_currentCamera;
    
    // In prespective view, we canchange the orientation of the view, but we can't change it
    // if we are in the other fixed view. So, in those views we can only do a pan.
    if (view->getPerspectiveViewType() == GlDisplay::Perspective)
    {
        m_panViewMode = e->modifiers() & Qt::ControlModifier ? true : false;
        m_rotateViewMode = !m_panViewMode;
        m_rotationRadius = (m_currentCamera->getPosition() - m_currentCamera->getTargetPoint()).length();
    }
    else
    {
        m_panViewMode = true;
        m_rotateViewMode = !m_panViewMode;
    }

}

void CommandBase::mouseReleaseEvent(QMouseEvent* /*e*/)
{
}

void CommandBase::mouseMoveEvent(QMouseEvent* e)
{
	GLdouble x = 0.0,
        y = 0.0,
        z = 0.0;
    GLfloat wz = 0.0f;
    
    glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), m_viewPort[3] - e->y(), wz, m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);
    
    
    Point3D currPoint = Point3D(x, y, z);
	Point3D currWinPoint = Point3D(e->x(), m_viewPort[3] - e->y(), wz);
    if (m_panViewMode)
    {
        Point3D newPos = m_currentPoint - currPoint;
        m_currentCamera->setPosition(newPos + m_currentCamera->getPosition());
        m_currentCamera->setTargetPoint(newPos + m_currentCamera->getTargetPoint());
        qDebug(qPrintable(m_currentCamera->getPosition().toString()));
    }
    else if (m_rotateViewMode)
    {
		float longitude, colatitude;
        Point3D newPos = m_currentWinPoint - currWinPoint;

		longitude = newPos.getX() / 500.0f; // m_currentCamera->getDistanceFromTarget();
		colatitude = newPos.getY() / 500.0f; //m_currentCamera->getDistanceFromTarget();
		m_currentCamera->setLongitude(longitude + m_currentCamera->getLongitude());
		m_currentCamera->setColatitude(colatitude + m_currentCamera->getColatitude());

		//qDebug("Longitude: %f Latitude: %f", m_currentCamera->getLongitude(), m_currentCamera->getColatitude());
    }
    m_currentPoint = currPoint;
	m_currentWinPoint = currWinPoint;
}
