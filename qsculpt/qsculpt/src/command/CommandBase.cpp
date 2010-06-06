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
#include "Stable.h"
#include "CommandBase.h"
#include <QtOpenGL>
#include <QMouseEvent>
#include "ConfigContainer.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "IDocument.h"
#include "ISurface.h"
#include "DocumentView.h"
#include "Camera.h"

CommandBase::CommandBase(ICommand* parent)
:	ICommand(parent),
	m_configContainer(new ConfigContainer()),
	m_isActive(false),
	m_rotationRadius(0.0),
	m_currentCamera(NULL),
	m_intialCameraState(new Camera),
	m_panViewMode(false),
	m_rotateViewMode(false)
{
}

CommandBase::CommandBase(const CommandBase& cpy)
:	ICommand(cpy),
	m_configContainer(new ConfigContainer()),
	m_isActive(cpy.m_isActive),
	m_rotationRadius(cpy.m_rotationRadius),
	m_currentCamera(new Camera),
	m_intialCameraState(new Camera),
	m_panViewMode(cpy.m_panViewMode),
	m_rotateViewMode(cpy.m_rotateViewMode)
{
	//*m_currentCamera = *cpy.m_currentCamera;
	//*m_intialCameraState = *cpy.m_intialCameraState;
}

CommandBase::CommandBase(const QString& text, ICommand* parent)
:	ICommand(text, parent),
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
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    GLdouble x, y, z;
    GLfloat wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
    glGetIntegerv(GL_VIEWPORT, m_viewPort);

    glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), m_viewPort[3] - e->y(), wz, m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

    m_initialWinPoint = Point3(e->x(), m_viewPort[3] - e->y(), wz);
    m_currentWinPoint = m_initialWinPoint;
    m_intialPoint = Point3(x, y, z);
    m_currentPoint = m_intialPoint;
    //qDebug("wz: %f %s", wz, qPrintable(m_intialPoint.toString()));

    m_currentCamera = view->getViewCamera();
    *m_intialCameraState = *m_currentCamera;

    // In prespective view, we allow the user to change the orientation of the view. 
    // In the other views, we only allow screen panning.
    if (view->getPerspectiveViewType() == GlView::Perspective)
    {
        m_panViewMode = e->modifiers() & Qt::ControlModifier ? true : false;
        m_rotateViewMode = !m_panViewMode;
        m_rotationRadius = (m_currentCamera->getPosition() - m_currentCamera->getTargetPoint()).norm();
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
	if (e->buttons() == Qt::NoButton)
	{
		return;
	}
    GLdouble x = 0.0,
        y = 0.0,
        z = 0.0;
    GLfloat wz = 0.0f;

    glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), m_viewPort[3] - e->y(), wz, m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);


    Point3 currPoint = Point3(x, y, z);
    Point3 currWinPoint = Point3(e->x(), m_viewPort[3] - e->y(), wz);
    if (m_panViewMode)
    {
        Point3 newPos = m_currentPoint - currPoint;
        m_currentCamera->setPosition(newPos + m_currentCamera->getPosition());
        m_currentCamera->setTargetPoint(newPos + m_currentCamera->getTargetPoint());
        //qDebug(qPrintable(m_currentCamera->getPosition().toString()));
    }
    else if (m_rotateViewMode)
    {
        float longitude, colatitude;
        Point3 newPos = m_currentWinPoint - currWinPoint;

        longitude = newPos.x() / 500.0f; // m_currentCamera->getDistanceFromTarget();
        colatitude = newPos.y() / 500.0f; //m_currentCamera->getDistanceFromTarget();
        m_currentCamera->setLongitude(longitude + m_currentCamera->getLongitude());
        m_currentCamera->setColatitude(colatitude + m_currentCamera->getColatitude());

        //qDebug("Longitude: %f Latitude: %f", m_currentCamera->getLongitude(), m_currentCamera->getColatitude());
    }
    m_currentPoint = currPoint;
    m_currentWinPoint = currWinPoint;
}

void CommandBase::paintGL()
{
    if (needsUserInteraction()) {
        glDisable(GL_LIGHTING);
        glPointSize(30);
        glBegin(GL_POINTS);
        glVertex3f(0, 0, 0);
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

