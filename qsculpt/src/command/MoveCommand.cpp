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
#include <QtGui/QMouseEvent>
#include <QtOpenGL/QtOpenGL>

#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/SceneNode.h>

#include "Console.h"
#include "DocumentView.h"
#include "IConfigContainer.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "command/MoveCommand.h"

struct TransformCommand::Impl
{
    bool _actionFinished; /**< Flag to indicate that the command
                           * is modifying the object. */
    Point3 _initial;      /**< Initial position of the object. */
    Point3 _delta; /**< Change of position between mouse
                    * movements */
    Point3 _final;         /**< Final postion of the object. */
    QPoint _mousePosition; /**< Initial mouse position */
    std::vector<SceneNode::weak_ptr> _objects; /**< Object list to move */

    Impl() {}

    Impl(const Impl& orig)
        : _actionFinished(false)
        , _initial(orig._initial)
        , _delta(orig._delta)
    {
    }
};

TransformCommand::TransformCommand()
    : CommandBase()
    , d_(new Impl)
{
    _configContainer->setInt(CONF_ACTION, Move);
    _configContainer->setInt(CONF_MOVE_AXIS, XYAxis);
    _configContainer->setInt(CONF_ROTATE_AXIS, XYAxis);
    _configContainer->setInt(CONF_SCALE_AXIS, XYAxis);

    _configContainer->setDouble(CONF_MOVE_X, 0);
    _configContainer->setDouble(CONF_MOVE_Y, 0.2);
    _configContainer->setDouble(CONF_MOVE_Z, 0.4);
    _configContainer->setDouble(CONF_ROTATE_X, 0);
    _configContainer->setDouble(CONF_ROTATE_Y, 0);
    _configContainer->setDouble(CONF_ROTATE_Z, 0);
    _configContainer->setDouble(CONF_SCALE_X, 1);
    _configContainer->setDouble(CONF_SCALE_Y, 1);
    _configContainer->setDouble(CONF_SCALE_Z, 1);
}

TransformCommand::TransformCommand(const TransformCommand& cpy)
    : CommandBase(cpy)
    , d_(new Impl(*cpy.d_))
{
}

TransformCommand::~TransformCommand() {}

ICommand* TransformCommand::clone() const
{
    return new TransformCommand(*this);
}

void TransformCommand::execute()
{
    double x = 0.0, y = 0.0, z = 0.0;
    Action action;
    int    count = 0;
    bool   ok    = true;

    // If the config container has a numeric key, then we assume it was
    // called by the console.
    if (_configContainer->containsKey("0"))
    {
        QString cmdName = _configContainer->getString("0");
        if (cmdName == "move")
            action = Move;
        if (cmdName == "rotate")
            action = Rotate;
        if (cmdName == "scale")
            action = Scale;

        x = _configContainer->getString("1").toDouble(&ok);
        if (!ok)
        {
            qDebug() << "error on command argument 1";
            return;
        }
        y = _configContainer->getString("2").toDouble(&ok);
        if (!ok)
        {
            qDebug() << "error on command argument 2";
            return;
        }
        z = _configContainer->getString("3").toDouble(&ok);
        if (!ok)
        {
            qDebug() << "error on command argument 3";
            return;
        }
        qDebug() << "arguments passed";
    }
    else
    {
        action = (Action)_configContainer->getInt(CONF_MOVE_AXIS);

        switch (action)
        {
        case Move:
            x = _configContainer->getDouble(CONF_MOVE_X);
            y = _configContainer->getDouble(CONF_MOVE_Y);
            z = _configContainer->getDouble(CONF_MOVE_Z);
            break;
        case Rotate:
            x = _configContainer->getDouble(CONF_ROTATE_X);
            y = _configContainer->getDouble(CONF_ROTATE_Y);
            z = _configContainer->getDouble(CONF_ROTATE_Z);
            break;
        case Scale:
            x = _configContainer->getDouble(CONF_SCALE_X);
            y = _configContainer->getDouble(CONF_SCALE_Y);
            z = _configContainer->getDouble(CONF_SCALE_Z);
            break;
        case AllActions:
            break;
        }
    }

    auto doc = g_pApp->getMainWindow()->getCurrentDocument();

    auto objects = doc->getSelectedObjects();
    switch (action)
    {
    default:
    case Move:
        count = objects.size();
        if (count > 0)
        {
            auto obj = objects[0].lock();
            if (obj)
            {
                d_->_initial = obj->transform().translation();
            }
        }
        for (int i = 0; i < count; i++)
        {
            //_final.setPoint( mx, my, mz);
            d_->_final.x() = x;
            d_->_final.y() = y;
            d_->_final.z() = z;

            //                objects[i]->displace(_initial - _final);
            auto obj = objects[i].lock();
            if (obj)
            {
                obj->transform().translation() += d_->_initial - d_->_final;
            }
        }
        CONSOLE()->write("move object");
        break;
    case Rotate:
        qDebug() << "Move command: rotate not implemented";
        //            count = objects.size();
        //            for (int i = 0; i < count; i++)
        //            {
        //                objects[i]->setOrientation(x, y, z);
        //            }
        break;
    case Scale:
        break;
    }
    g_pApp->getMainWindow()->getCurrentView()->updateView();
}

void TransformCommand::activate(bool active)
{
    CommandBase::activate(active);

    Action action = (Action)_configContainer->getInt(CONF_MOVE_AXIS);
    if (d_->_actionFinished)
    {
        int count = d_->_objects.size();
        for (int i = 0; i < count; i++)
        {
            if (action == Move)
            {
                auto obj = d_->_objects[i].lock();
                obj->transform().translation() += d_->_initial - d_->_final;
            }
        }
    }
    d_->_actionFinished = false;

    d_->_objects.clear();
}

void TransformCommand::mouseMoveEvent(QMouseEvent* e)
{
    // CommandBase::mouseMoveEvent(e);

    d_->_mousePosition = e->pos();
    int     height     = g_pApp->getMainWindow()->getCurrentView()->height();
    Camera* cam =
        g_pApp->getMainWindow()->getCurrentView()->getViewCamera().get();
    Point3 camPos = cam->eyeToWorld(d_->_mousePosition.x(),
                                    height - d_->_mousePosition.y(), 0);

    Point3 delta = camPos - d_->_initial;

    Point3 d;
    switch (_configContainer->getInt(CONF_MOVE_AXIS))
    {
    case XAxis:
        d.x() = delta.x();
        break;

    case YAxis:
        d.y() = delta.y();
        break;

    case ZAxis:
        d.z() = delta.z();
        break;

    default:
    case XYAxis:
        d.x() = delta.x();
        d.y() = delta.y();
        break;

    case XZAxis:
        d.x() = delta.x();
        d.z() = delta.y();
        break;

    case YZAxis:
        d.z() = delta.z();
        d.y() = delta.y();
        break;
    }
    int count = d_->_objects.size();
    for (int i = 0; i < count; i++)
    {
        auto obj = d_->_objects[i].lock();
        obj->transform().translation() += d;
    }
    d_->_initial = d_->_initial + delta;
}

void TransformCommand::mousePressEvent(QMouseEvent* e)
{
    // CommandBase::mousePressEvent(e);

    d_->_mousePosition  = e->pos();
    d_->_actionFinished = true;

    float wz = 0.0f;

    int height = g_pApp->getMainWindow()->getCurrentView()->height();

    glReadPixels(e->x(), height - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT,
                 &wz);
    Camera* cam =
        g_pApp->getMainWindow()->getCurrentView()->getViewCamera().get();
    d_->_initial = cam->eyeToWorld(d_->_mousePosition.x(),
                                   height - d_->_mousePosition.y(), 0);
    d_->_final = d_->_initial;
    // qDebug("Initial position: %s", qPrintable(_initial.toString()));

    auto doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc->getSelectedObjects().size() > 0)
    {
        auto list = doc->getSelectedObjects();
        d_->_objects.insert(d_->_objects.end(), list.begin(), list.end());
    }
}

void TransformCommand::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    // CommandBase::mouseReleaseEvent(e);

    // qDebug("Final position: %s", qPrintable(_final.toString()));

    d_->_actionFinished = false;
    emit executed();
}

void TransformCommand::paintGL(GlCanvas* c) { Q_UNUSED(c); }
