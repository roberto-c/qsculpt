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
#include "IObject3D.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

TransformCommand::TransformCommand()
    : CommandBase(),
    m_actionFinished(false)
{
    m_configContainer->setInt(CONF_ACTION, Move);
    m_configContainer->setInt(CONF_MOVE_AXIS, XYAxis);
    m_configContainer->setInt(CONF_ROTATE_AXIS, XYAxis);
    m_configContainer->setInt(CONF_SCALE_AXIS, XYAxis);

    m_configContainer->setDouble( CONF_MOVE_X, 0);
    m_configContainer->setDouble( CONF_MOVE_Y, 0.2);
    m_configContainer->setDouble( CONF_MOVE_Z, 0.4);
    m_configContainer->setDouble( CONF_ROTATE_X, 0);
    m_configContainer->setDouble( CONF_ROTATE_Y, 0);
    m_configContainer->setDouble( CONF_ROTATE_Z, 0);
    m_configContainer->setDouble( CONF_SCALE_X, 1);
    m_configContainer->setDouble( CONF_SCALE_Y, 1);
    m_configContainer->setDouble( CONF_SCALE_Z, 1);
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

    mx = m_configContainer->getDouble(CONF_MOVE_X);
    my = m_configContainer->getDouble(CONF_MOVE_Y);
    mz = m_configContainer->getDouble(CONF_MOVE_Z);

    rx = m_configContainer->getDouble(CONF_ROTATE_X);
    ry = m_configContainer->getDouble(CONF_ROTATE_Y);
    rz = m_configContainer->getDouble(CONF_ROTATE_Z);

    sx = m_configContainer->getDouble(CONF_SCALE_X);
    sy = m_configContainer->getDouble(CONF_SCALE_Y);
    sz = m_configContainer->getDouble(CONF_SCALE_Z);

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    Action action = (Action)m_configContainer->getInt(CONF_MOVE_AXIS);
    QList<IObject3D*> objects = doc->getSelectedObjects();
    switch(action)
    {
        default:
        case Move:
            count = objects.size();
            if (count > 0)
                m_initial = objects[0]->getPosition();
            for (int i = 0; i < count; i++)
            {
                m_final.setPoint( mx, my, mz);
                if (count == 1)
                    objects[i]->setPosition( m_final);
                else
                    objects[i]->displace(m_initial - m_final);
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

    Action action = (Action)m_configContainer->getInt(CONF_MOVE_AXIS);
    if (m_actionFinished)
    {
        int count = m_objects.count();
        for (int i = 0; i < count; i++)
        {
            if (action == Move)
                m_objects[i]->displace(m_initial - m_final);
            else if (action == Rotate)
                m_objects[i]->rotate(0 ,0 ,0);
        }
    }
    m_actionFinished = false;

    m_objects.clear();

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (doc->getObjectsCount() > 0 )
    {
        m_objects += doc->getSelectedObjects();
        m_initial = m_objects.first()->getPosition();
    }
}

void TransformCommand::mouseMoveEvent(QMouseEvent* e)
{
    //CommandBase::mouseMoveEvent(e);

    double dx = 0.0, dy = 0.0, dz = 0.0;
    double x = 0.0, y = 0.0, z = 0.0;

    m_mousePosition = e->pos();
    double modelMatrix[16], projMatrix[16];
    GLint viewPort[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewPort);
    gluUnProject(m_mousePosition.x(), viewPort[3] - m_mousePosition.y(), 0, modelMatrix, projMatrix, viewPort, &x, &y, &z);

    //m_final = Point3D(x, y, z);
    Point3D delta = Point3D(x, y, z) - m_initial;
    qDebug("Delta: %s", qPrintable(delta.toString()));
    dx = delta.getX();
    dy = delta.getY();

    Point3D d;
    switch(m_configContainer->getInt(CONF_MOVE_AXIS))
    {
        case XAxis:
            d.setX(dx);
            break;

        case YAxis:
            d.setY(dy);
            break;

        case ZAxis:
            d.setZ(dz);
            break;

        default:
        case XYAxis:
            d.setX(dx);
            d.setY(dy);
            break;

        case XZAxis:
            d.setX(dx);
            d.setZ(dy);
            break;

        case YZAxis:
            d.setZ(dz);
            d.setY(dy);
            break;
    }
    int count = m_objects.count();
    for (int i = 0; i < count; i++)
    {
        IObject3D* obj = m_objects[i];
        obj->setPosition(obj->getPosition() + d);
    }
    m_initial = m_initial + delta;
}

void TransformCommand::mousePressEvent(QMouseEvent* e)
{
    //CommandBase::mousePressEvent(e);

    m_mousePosition = e->pos();
    m_actionFinished = true;

    double x, y, z;
    float wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
    glGetIntegerv(GL_VIEWPORT, m_viewPort);
    glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(m_mousePosition.x(), m_viewPort[3] - m_mousePosition.y(), 0, m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

    m_initial = Point3D(x, y, z);
    m_final = m_initial;
    qDebug("Initial position: %s", qPrintable(m_initial.toString()));

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc->getSelectedObjects().size() > 0 )
    {
        m_objects += doc->getSelectedObjects();
    }
}

void TransformCommand::mouseReleaseEvent(QMouseEvent* e)
{
    //CommandBase::mouseReleaseEvent(e);

    qDebug("Final position: %s", qPrintable(m_final.toString()));

    m_actionFinished = false;
    emit executed();
}

