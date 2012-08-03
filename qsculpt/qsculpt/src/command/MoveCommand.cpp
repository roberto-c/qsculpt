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

#include "command/MoveCommand.h"
#include "IDocument.h"
#include "IConfigContainer.h"
#include "ISurface.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "Console.h"
#include "DocumentView.h"
#include "SceneNode.h"

struct TransformCommand::Impl
{
    bool                        _actionFinished;   /**< Flag to indicate that the command
                                            * is modifying the object. */
    Point3                      _initial;          /**< Initial position of the object. */
    Point3                      _delta;            /**< Change of position between mouse
                                            * movements */
    Point3                      _final;            /**< Final postion of the object. */
    QPoint                      _mousePosition;    /**< Initial mouse position */
    QList<SceneNode::weak_ptr>   _objects;          /**< Object list to move */
    
    Impl(){}
    
    Impl(const Impl& orig) 
    :   _actionFinished(false), 
        _initial(orig._initial),
        _delta(orig._delta){}
};

TransformCommand::TransformCommand()
    : CommandBase(),
    d_(new Impl)
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

TransformCommand::TransformCommand(const TransformCommand& cpy)
: CommandBase(cpy),
d_(new Impl(*cpy.d_))
{
}

TransformCommand::~TransformCommand()
{
}

ICommand* TransformCommand::clone() const 
{
    return new TransformCommand(*this);
}

void TransformCommand::execute()
{
    double x = 0.0, y = 0.0, z = 0.0;
    Action action;
    int count= 0;
    bool ok = true;

    
    // If the config container has a numeric key, then we assume it was
    // called by the console.
    if (_configContainer->containsKey("0"))
    {
        QString cmdName = _configContainer->getString("0"); 
        if (cmdName == "move") action = Move;
        if (cmdName == "rotate") action = Rotate;
        if (cmdName == "scale") action = Scale;
        
        x = _configContainer->getString("1").toDouble(&ok);
        if (!ok) {
            qDebug() << "error on command argument 1";
            return;
        }
        y = _configContainer->getString("2").toDouble(&ok);
        if (!ok) {
            qDebug() << "error on command argument 2";
            return;
        }
        z = _configContainer->getString("3").toDouble(&ok);
        if (!ok) {
            qDebug() << "error on command argument 3";
            return;
        }        
        qDebug() << "arguments passed";
    }
    else
    {
        action = (Action)_configContainer->getInt(CONF_MOVE_AXIS);
        
        switch(action) {
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

    QList<SceneNode::weak_ptr> objects = doc->getSelectedObjects();
    switch(action)
    {
        default:
        case Move:
            count = objects.size();
            if (count > 0) 
            {
                auto obj = objects[0].lock();
                if (obj) {
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
                if (obj) {
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
        int count = d_->_objects.count();
        for (int i = 0; i < count; i++)
        {
            if (action == Move) {
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
    //CommandBase::mouseMoveEvent(e);

    double dx = 0.0, dy = 0.0, dz = 0.0;
    double x = 0.0, y = 0.0, z = 0.0;

    d_->_mousePosition = e->pos();
    double modelMatrix[16], projMatrix[16];
    GLint viewPort[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewPort);
    gluUnProject(d_->_mousePosition.x(), viewPort[3] - d_->_mousePosition.y(), 0, modelMatrix, projMatrix, viewPort, &x, &y, &z);

    //_final = Point3D(x, y, z);
    Point3 delta = Point3(x, y, z) - d_->_initial;
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
    int count = d_->_objects.count();
    for (int i = 0; i < count; i++)
    {
        auto obj = d_->_objects[i].lock();
        obj->transform().translation() += d;
    }
    d_->_initial = d_->_initial + delta;
}

void TransformCommand::mousePressEvent(QMouseEvent* e)
{
    //CommandBase::mousePressEvent(e);

    d_->_mousePosition = e->pos();
    d_->_actionFinished = true;

    double x, y, z;
    float wz = 0.0f;

    glGetDoublev(GL_MODELVIEW_MATRIX, _modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, _projMatrix);
    glGetIntegerv(GL_VIEWPORT, _viewPort);
    glReadPixels(e->x(), _viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(d_->_mousePosition.x(), _viewPort[3] - d_->_mousePosition.y(), 0, _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);

    d_->_initial = Point3(x, y, z);
    d_->_final = d_->_initial;
    //qDebug("Initial position: %s", qPrintable(_initial.toString()));

    auto doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc->getSelectedObjects().size() > 0 )
    {
        d_->_objects += doc->getSelectedObjects();
    }
}

void TransformCommand::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    //CommandBase::mouseReleaseEvent(e);

    //qDebug("Final position: %s", qPrintable(_final.toString()));

    d_->_actionFinished = false;
    emit executed();
}

void TransformCommand::paintGL(GlCanvas *c)
{
    Q_UNUSED(c);
}
