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
#include "command/orbitcommand.h"
#include "GlView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"
#include "Document.h"
#include "Scene.h"
#include "SceneNode.h"
#include "subdivision/Box.h"
#include "subdivision/Sphere.h"
#include "FlatRenderer.h"
#include "Eigen/Geometry"
#include "BufferObject.h"
#include "math/Utils.h"

struct OrbitCommand::Impl
{
    Point3  initial,endPoint, centerPoint;
    Vector3 startVector, endVector;
    Vector3 initialVector;
    float   radius;
    float   startAngle;
    float   endAngle;
    bool    draw;
    QList<ISurface*> selectedObj;
    Document doc;
    IRenderer *renderer;
    Document grid;
    Eigen::Quaternionf rot;
    Eigen::Transform3f t;
    
    Impl() : 
    initial(Point3())
    , radius(0)
    , startAngle(0)
    , endAngle(360)
    , draw(false)
    , renderer(new FlatRenderer)
    {
        qDebug() << "OrbitCommand::Impl constructed";
    }
    
    Impl(const Impl& cpy) :
    initial(cpy.initial)
    , endPoint(cpy.endPoint)
    , startVector(cpy.startVector)
    , endVector(cpy.endVector)
    , radius(cpy.radius)
    , startAngle(cpy.startAngle)
    , endAngle(cpy.endAngle)
    , draw(false)
    , selectedObj(cpy.selectedObj)
    , renderer(new FlatRenderer)
    {
        qDebug() << "OrbitCommand::Impl constructed(cpy)";
    }
    
    ~Impl() {
        qDebug() << "OrbitCommand::Impl destruyed";
    }
    
    void setup();
};

void OrbitCommand::Impl::setup()
{
    SceneNode * root = new SceneNode;
    
    SurfaceNode *surface = new SurfaceNode(new Box, root);
    surface->transform() *= Eigen::Translation<float,3>(1, 0, 0);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    
    surface = new SurfaceNode(new Box, root);
    surface->transform() *= Eigen::Translation<float,3>(0, 1, 0);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    
    surface = new SurfaceNode(new Box, root);
    surface->transform() *= Eigen::Translation<float,3>(0, 0, 1);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    
    root->transform() *= Eigen::Translation<float,3>(0,0,0);
    doc.scene()->appendRow(root);
    
    surface = new SurfaceNode(new Plane(1,1));
    //surface = new SurfaceNode(new Sphere());
    grid.scene()->appendRow(surface);
}

OrbitCommand::OrbitCommand(ICommand *parent)
    : CommandBase(parent)
    , _d(new Impl)
{
    _d->setup();
}

OrbitCommand::OrbitCommand(const OrbitCommand &cpy)
    : CommandBase(cpy)
    , _d(new Impl(*cpy._d))
{
    _d->setup();
}

OrbitCommand::~OrbitCommand()
{
}

ICommand* OrbitCommand::clone() const
{
    return new OrbitCommand(*this);
}

QWidget* OrbitCommand::getOptionsWidget() {
    return NULL;
}

void OrbitCommand::mousePressEvent(QMouseEvent *e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    
    _d->selectedObj = view->getDocument()->getSelectedObjects();
    
    if (_d->selectedObj.size() > 0) {
        _d->draw = true;
        // inital point of the arc is the center of the screen
        _d->initial.x() = view->getCanvas()->width() / 2;
        _d->initial.y() = view->getCanvas()->height() / 2;
        // start vector is the vector from the center of the screen to the
        // current mouse position.
        _d->startVector.x() = e->x();
        _d->startVector.y() = view->getCanvas()->height() - e->y();
        _d->startVector = _d->startVector - _d->initial;
        // Normalize the vector for doing angle calculations between vectors
        _d->startVector.normalize();
        // for now, end vector is the same as the start vector
        _d->endVector = _d->startVector;
        
        // Calculate the angle of the startVector with respect to X axis
        Vector3 xaxis(1, 0,0);
        _d->startAngle = - atan2(xaxis.y(),xaxis.x()) 
            + atan2(_d->endVector.y(),_d->endVector.x());
        // Use degrees and clamp to [0,360] range
        _d->startAngle*=180.0f / M_PI;
        _d->startAngle = _d->startAngle < 0 ? _d->startAngle + 360 : _d->startAngle;
    } else {
        _d->draw = true;
        _d->initial.x() = view->getCanvas()->width() / 2;
        _d->initial.y() = view->getCanvas()->height() / 2;
        
        _d->centerPoint = view->getViewCamera()->eyeToWorld(view->getCanvas()->width() / 2,
                                                 view->getCanvas()->height() / 2,
                                                 0.5f);
        Vector3 v2 = view->getViewCamera()->eyeToWorld(e->x(),
                                                       view->getCanvas()->height() - e->y(),
                                                       0.1f);
        _d->startVector = v2 - _d->centerPoint;
        _d->startVector.normalize();
        _d->initialVector = _d->startVector;
        //qDebug() << "Initial " << toString(_d->initialVector);
    }
}

void OrbitCommand::mouseReleaseEvent(QMouseEvent *e)
{
    if (_d->selectedObj.size() > 0) {
        
    } else {
//        Iterator<SceneNode> it = _d->grid.scene()->iterator();
//        while (it.hasNext()) {
//            SceneNode* node = &it.next();
//            node->transform() = _d->t;
//        }
    }
    
    _d->draw = false;
}

void OrbitCommand::mouseMoveEvent(QMouseEvent *e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    if (_d->selectedObj.size() > 0) {        
        // Get the end vector, from the start point to the current mouse position
        _d->endVector = Point3(e->x(), view->getCanvas()->height() - e->y(), 0) 
                        - _d->initial;
        // set the radius to the distance between center and end point
        _d->radius = _d->endVector.norm(); 
        // normalize the vector to make angle calculations
        _d->endVector.normalize();
        // Get the angle of the end vector with respect to the start vector.
        _d->endAngle = atan2(_d->endVector.y(),_d->endVector.x()) 
            - atan2(_d->startVector.y(),_d->startVector.x());
        // Use degrees and use a range between [0, 360]
        _d->endAngle = _d->endAngle * 180.0f / M_PI; 
        _d->endAngle = _d->endAngle < 0 ? _d->endAngle + 360 : _d->endAngle;
    } else {
        //Vector3 tmp2 = _d->endPoint;
//        Vector3 v1 = view->getViewCamera()->eyeToWorld(Point3(view->getCanvas()->width() / 2,
//                                                               view->getCanvas()->height() / 2,
//                                                               0.5f));
        Vector3 v2 = view->getViewCamera()->eyeToWorld(e->x(),
                                                        view->getCanvas()->height() - e->y(),
                                                        0.1f);
        
        _d->endVector = v2 - _d->centerPoint;
        _d->endVector.normalize();
//        Vector3 tmp = _d->startVector.cross(_d->endVector);
//        float dot = _d->startVector.dot(_d->endVector);
//        if (dot != dot) {
//            return;
//        }
//        dot = math::clamp(dot, -1.f, 1.f);
//        float angle = acos(dot) / 200;
//        std::swap(_d->startVector, _d->endVector);
//        
//        Eigen::Quaternionf qtmp = Eigen::Quaternionf(angle, tmp.x(), tmp.y(), tmp.z());
//        qtmp.normalize();
//        
//        Iterator<SceneNode> it = _d->doc.scene()->iterator();
//        while (it.hasNext()) {
//            SceneNode* node = &it.next();
//            node->transform().rotate(qtmp);
//        }
    }
}

void OrbitCommand::paintGL(GlCanvas *c)
{
    static QBrush brush(QColor(0, 120, 220, 180));
    static QPen pen(QColor(0, 220, 220, 180));
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    
    if (_d->draw) {
        if (_d->selectedObj.size() > 0) {
            float rad1 = _d->radius < 100 ? 100.0f : _d->radius;
            float rad2 = rad1 - 5;
            c->setBrush(brush);
            c->setPen(pen);
            c->disable(GL_DEPTH_TEST);
            c->drawArc(_d->initial,0,360,
                       rad1, rad1, rad2, rad2);
            c->drawArc(_d->initial,_d->startAngle,_d->endAngle,
                       rad1, rad1, rad2, rad2);
            c->enable(GL_DEPTH_TEST);
            
            c->drawLine2D(_d->initial, _d->initial + 100*(_d->startVector));
            c->drawLine2D(_d->initial, _d->initial + 100*(_d->endVector));
        } else {
            c->setBrush(brush);
            c->setPen(pen);
            c->disable(GL_DEPTH_TEST);
            c->begin(GL_LINES);
            c->drawLine(Point3(0,0,0), _d->initialVector*2);
            qDebug() << "Initial " << toString(_d->initialVector);
            //c->drawLine(Point3(0,0,0), Point3(1.5f, 0, 0));
            c->end();
            c->enable(GL_DEPTH_TEST);
        }
    }
    c->drawScene(_d->grid.scene());
    c->drawScene(_d->doc.scene());
}