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
#include "GlView.h"

CommandBase::CommandBase(ICommand* parent)
:	ICommand(parent),
	_configContainer(new ConfigContainer()),
	_isActive(false),
	_rotationRadius(0.0),
	_currentCamera(NULL),
	_intialCameraState(new Camera),
	_panViewMode(false),
	_rotateViewMode(false)
{
}

CommandBase::CommandBase(const CommandBase& cpy)
:	ICommand(cpy),
	_configContainer(new ConfigContainer()),
	_isActive(cpy._isActive),
	_rotationRadius(cpy._rotationRadius),
	_currentCamera(new Camera),
	_intialCameraState(new Camera),
	_panViewMode(cpy._panViewMode),
	_rotateViewMode(cpy._rotateViewMode)
{
	//*m_currentCamera = *cpy.m_currentCamera;
	//*m_intialCameraState = *cpy.m_intialCameraState;
}

CommandBase::CommandBase(const QString& text, ICommand* parent)
:	ICommand(text, parent),
	_configContainer(new ConfigContainer()),
	_isActive(false),
	_rotationRadius(0.0),
	_currentCamera(NULL),
	_intialCameraState(new Camera),
	_panViewMode(false),
	_rotateViewMode(false)
{
}

CommandBase::~CommandBase()
{
}

IConfigContainer& CommandBase::getConfig() const
{
    return *_configContainer;
}

void CommandBase::activate(bool active)
{
    _isActive = active;
}

bool CommandBase::isActive()
{
    return _isActive;
}

void CommandBase::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    GLdouble x, y, z;
    GLfloat wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, _modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, _projMatrix);
    glGetIntegerv(GL_VIEWPORT, _viewPort);

    glReadPixels(e->x(), _viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), _viewPort[3] - e->y(), wz, _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);

    _initialWinPoint = Point3(e->x(), _viewPort[3] - e->y(), wz);
    _currentWinPoint = _initialWinPoint;
    _intialPoint = Point3(x, y, z);
    _currentPoint = _intialPoint;
    //qDebug("wz: %f %s", wz, qPrintable(m_intialPoint.toString()));

    _currentCamera = view->getViewCamera();
    *_intialCameraState = *_currentCamera;

    // In prespective view, we allow the user to change the orientation of the view. 
    // In the other views, we only allow screen panning.
    if (view->getPerspectiveViewType() == GlCanvas::Perspective)
    {
        _panViewMode = e->modifiers() & Qt::ControlModifier ? true : false;
        _rotateViewMode = !_panViewMode;
        _rotationRadius = (_currentCamera->getPosition() - _currentCamera->getTargetPoint()).norm();
    }
    else
    {
        _panViewMode = true;
        _rotateViewMode = !_panViewMode;
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

    glReadPixels(e->x(), _viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(e->x(), _viewPort[3] - e->y(), wz, _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);


    Point3 currPoint = Point3(x, y, z);
    Point3 currWinPoint = Point3(e->x(), _viewPort[3] - e->y(), wz);
    if (_panViewMode)
    {
        Point3 newPos = _currentPoint - currPoint;
        _currentCamera->setPosition(newPos + _currentCamera->getPosition());
        _currentCamera->setTargetPoint(newPos + _currentCamera->getTargetPoint());
        //qDebug(qPrintable(m_currentCamera->getPosition().toString()));
    }
    else if (_rotateViewMode)
    {
        float longitude, colatitude;
        Point3 newPos = _currentWinPoint - currWinPoint;

        longitude = newPos.x() / 500.0f; // m_currentCamera->getDistanceFromTarget();
        colatitude = newPos.y() / 500.0f; //m_currentCamera->getDistanceFromTarget();
        _currentCamera->setLongitude(longitude + _currentCamera->getLongitude());
        _currentCamera->setColatitude(colatitude + _currentCamera->getColatitude());

        //qDebug("Longitude: %f Latitude: %f", m_currentCamera->getLongitude(), m_currentCamera->getColatitude());
    }
    _currentPoint = currPoint;
    _currentWinPoint = currWinPoint;
}

void CommandBase::paintGL(GlCanvas *c)
{

}

