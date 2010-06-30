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
#include "MoveCommand.h"
#include <QMouseEvent>
#include <QtOpenGL>
#include "IDocument.h"
#include "IConfigContainer.h"
#include "ISurface.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

TransformCommand::TransformCommand()
    : CommandBase(),
    _actionFinished(false)
{
    _configContainer->setInt(CONF_ACTION, Move);
    _configContainer->setInt(CONF_MOVE_AXIS, XYAxis);
    _configContainer->setInt(CONF_ROTATE_AXIS, XYAxis);
    _configContainer->setInt(CONF_SCALE_AXIS, XYAxis);

    _configContainer->setDouble( CONF_MOVE_X, 0);
    _configContainer->setDouble( CONF_MOVE_Y, 0.2);
    _configContainer->setDouble( CONF_MOVE_Z, 0.4);
    _configContainer->setDouble( CONF_ROTATE_X, 0);
    _configContainer->setDouble( CONF_ROTATE_Y, 0);
    _configContainer->setDouble( CONF_ROTATE_Z, 0);
    _configContainer->setDouble( CONF_SCALE_X, 1);
    _configContainer->setDouble( CONF_SCALE_Y, 1);
    _configContainer->setDouble( CONF_SCALE_Z, 1);
}


TransformCommand::~TransformCommand()
{
}

void TransformCommand::execute()
{
    double mx = 0.0, my = 0.0, mz = 0.0;
    double rx = 0.0, ry = 0.0, rz = 0.0;
    double sx = 0.0, sy = 0.0, sz = 0.0;
    int count= 0;

    mx = _configContainer->getDouble(CONF_MOVE_X);
    my = _configContainer->getDouble(CONF_MOVE_Y);
    mz = _configContainer->getDouble(CONF_MOVE_Z);

    rx = _configContainer->getDouble(CONF_ROTATE_X);
    ry = _configContainer->getDouble(CONF_ROTATE_Y);
    rz = _configContainer->getDouble(CONF_ROTATE_Z);

    sx = _configContainer->getDouble(CONF_SCALE_X);
    sy = _configContainer->getDouble(CONF_SCALE_Y);
    sz = _configContainer->getDouble(CONF_SCALE_Z);

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    Action action = (Action)_configContainer->getInt(CONF_MOVE_AXIS);
    QList<ISurface*> objects = doc->getSelectedObjects();
    switch(action)
    {
        default:
        case Move:
            count = objects.size();
            if (count > 0)
                _initial = objects[0]->getPosition();
            for (int i = 0; i < count; i++)
            {
                //_final.setPoint( mx, my, mz);
				_final.x() = mx;
				_final.y() = my;
				_final.z() = mz;
                if (count == 1)
                    objects[i]->setPosition( _final);
                else
                    objects[i]->displace(_initial - _final);
            }
            break;
        case Rotate:
            count = objects.size();
            for (int i = 0; i < count; i++)
            {
                objects[i]->rotate(rx, ry, rz);
            }
            break;
        case Scale:
            break;
    }
}

void TransformCommand::activate(bool active)
{
    CommandBase::activate(active);

    Action action = (Action)_configContainer->getInt(CONF_MOVE_AXIS);
    if (_actionFinished)
    {
        int count = _objects.count();
        for (int i = 0; i < count; i++)
        {
            if (action == Move)
                _objects[i]->displace(_initial - _final);
            else if (action == Rotate)
                _objects[i]->rotate(0 ,0 ,0);
        }
    }
    _actionFinished = false;

    _objects.clear();

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (doc->getObjectsCount() > 0 )
    {
        _objects += doc->getSelectedObjects();
        _initial = _objects.first()->getPosition();
    }
}

void TransformCommand::mouseMoveEvent(QMouseEvent* e)
{
    //CommandBase::mouseMoveEvent(e);

    double dx = 0.0, dy = 0.0, dz = 0.0;
    double x = 0.0, y = 0.0, z = 0.0;

    _mousePosition = e->pos();
    double modelMatrix[16], projMatrix[16];
    GLint viewPort[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewPort);
    gluUnProject(_mousePosition.x(), viewPort[3] - _mousePosition.y(), 0, modelMatrix, projMatrix, viewPort, &x, &y, &z);

    //_final = Point3D(x, y, z);
    Point3 delta = Point3(x, y, z) - _initial;
    //qDebug("Delta: %s", qPrintable(delta.toString()));
    dx = delta.x();
    dy = delta.y();

    Point3 d;
    switch(_configContainer->getInt(CONF_MOVE_AXIS))
    {
        case XAxis:
            d.x() = dx;
            break;

        case YAxis:
            d.y() = dy;
            break;

        case ZAxis:
            d.z() = dz;
            break;

        default:
        case XYAxis:
            d.x() = dx;
            d.y() = dy;
            break;

        case XZAxis:
            d.x() = dx;
            d.z() = dy;
            break;

        case YZAxis:
            d.z() = dz;
            d.y() = dy;
            break;
    }
    int count = _objects.count();
    for (int i = 0; i < count; i++)
    {
        ISurface* obj = _objects[i];
        obj->setPosition(obj->getPosition() + d);
    }
    _initial = _initial + delta;
}

void TransformCommand::mousePressEvent(QMouseEvent* e)
{
    //CommandBase::mousePressEvent(e);

    _mousePosition = e->pos();
    _actionFinished = true;

    double x, y, z;
    float wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, _modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, _projMatrix);
    glGetIntegerv(GL_VIEWPORT, _viewPort);
    glReadPixels(e->x(), _viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(_mousePosition.x(), _viewPort[3] - _mousePosition.y(), 0, _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);

    _initial = Point3(x, y, z);
    _final = _initial;
    //qDebug("Initial position: %s", qPrintable(_initial.toString()));

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc->getSelectedObjects().size() > 0 )
    {
        _objects += doc->getSelectedObjects();
    }
}

void TransformCommand::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    //CommandBase::mouseReleaseEvent(e);

    //qDebug("Final position: %s", qPrintable(_final.toString()));

    _actionFinished = false;
    emit executed();
}

