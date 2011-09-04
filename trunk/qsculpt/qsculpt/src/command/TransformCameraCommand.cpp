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
#include "command/TransformCameraCommand.h"
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
#include "geometry/Ray.h"
#include "geometry/Sphere.h"
#include "math/Utils.h"


struct TransformCameraCommand::Impl
{
    Point3  initial,endPoint, centerPoint;
    Vector3 startVector, endVector;
    Vector3 initialVector;
    float   radius;
    float   startAngle;
    float   endAngle;
    bool    draw;
    QList<SceneNode*> selectedObj;
    Document doc;
    QScopedPointer<IRenderer> renderer;
    Eigen::Quaternionf rot;
    Eigen::Transform3f t;
    geometry::Ray ray;
    geometry::Sphere sphere;
    Camera  camera;
    std::vector<Eigen::Affine3f> originalTransform;
    
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

void TransformCameraCommand::Impl::setup()
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
    doc.scene()->add(root);
}


TransformCameraCommand::TransformCameraCommand(): CommandBase(), _d(new Impl)
{
}

TransformCameraCommand::TransformCameraCommand(const TransformCameraCommand & cpy)
: CommandBase(cpy),
_d(new Impl(*cpy._d))
{
    
}

TransformCameraCommand::~TransformCameraCommand()
{
}

void TransformCameraCommand::activate(bool /*active*/)
{
}

void TransformCameraCommand::execute()
{
    CommandBase::execute();
}

void TransformCameraCommand::mouseMoveEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    if (_d->selectedObj.size() > 0) {        
        Vector3 v1 = _d->camera.eyeToWorld(e->x(),
                                           view->getCanvas()->height() - e->y(),
                                           0.f);
        
        Vector3 v2 = _d->camera.eyeToWorld(e->x(),
                                           view->getCanvas()->height() - e->y(),
                                           0.8f);
        _d->ray.origin() = v1;
        _d->ray.direction() = v2 - v1;
        _d->ray.direction().normalize();
        qDebug() << "o = " << toString(_d->ray.origin()) << " d = " << toString(_d->ray.direction());
        if (_d->ray.intersect(_d->sphere, &_d->endVector) < 0 ) {
            qDebug() << "Intersection failed";
            return;
        }
        
        _d->endVector.normalize();
        qDebug() << "End " << toString(_d->endVector);
        
        Vector3 axis = _d->startVector.cross(_d->endVector);
        axis.normalize();
        float dot = _d->startVector.dot(_d->endVector);
        if (dot != dot) {
            return;
        }
        dot = math::clamp(dot, -1.f, 1.f);
        float angle = acos(dot);
        std::swap(_d->startVector, _d->endVector);
        
        Eigen::AngleAxisf rot = Eigen::AngleAxisf(angle, axis);        
        Iterator<SceneNode> it = _d->doc.scene()->iterator();
        while (it.hasNext()) {
            SceneNode* node = &it.next();
            node->transform().rotate(rot);
        }
        _d->selectedObj[0]->setTransform(_d->selectedObj[0]->transform().rotate(rot));
    }
}

void TransformCameraCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    
    _d->selectedObj = view->getDocument()->getSelectedObjects();
    
    if (_d->selectedObj.size() > 0) {
        _d->draw = true;
        
        _d->originalTransform.clear();
        _d->originalTransform.push_back(_d->selectedObj[0]->transform());
        
        _d->camera.setViewport(0, 0, view->getCanvas()->width(),
                               view->getCanvas()->height());
        float aspect = view->getCanvas()->width() / view->getCanvas()->height();
        _d->camera.setOrthoMatrix(-aspect, aspect, -1, 1, -1, 1);
        _d->camera.setTargetPoint( Point3( 0, 0, 0) );
        _d->camera.setOrientationVector(Point3( 0, 1, 0) );
        _d->camera.setPosition( Point3( 0, 0, 1));
        
        _d->draw = true;
        _d->initial.x() = view->getCanvas()->width() / 2;
        _d->initial.y() = view->getCanvas()->height() / 2;
        
        _d->centerPoint = _d->camera.eyeToWorld(view->getCanvas()->width() / 2,
                                                view->getCanvas()->height() / 2,
                                                0.5f);
        
        Vector3 v1 = _d->camera.eyeToWorld(e->x(),
                                           view->getCanvas()->height() - e->y(),
                                           0.f);
        
        Vector3 v2 = _d->camera.eyeToWorld(e->x(),
                                           view->getCanvas()->height() - e->y(),
                                           0.8f);
        _d->ray.origin() = v1;
        _d->ray.direction() = v2 - v1;
        _d->ray.direction().normalize();
        qDebug() << "o = " << toString(_d->ray.origin()) << " d = " << toString(_d->ray.direction());
        if (_d->ray.intersect(_d->sphere, &_d->startVector) >= 0) {
            _d->startVector.normalize();
            _d->initialVector = _d->startVector;
            qDebug() << "Initial " << toString(_d->initialVector);
        }
    }
}

void TransformCameraCommand::mouseReleaseEvent(QMouseEvent* e)
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

void TransformCameraCommand::undo()
{
    CommandBase::undo();
}

void TransformCameraCommand::paintGL(GlCanvas *c)
{
    static QBrush brush(QColor(0, 120, 220, 180));
    static QPen pen(QColor(0, 220, 220, 180));
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    
    if (_d->draw) {
        glClear(GL_DEPTH_BUFFER_BIT);
        c->drawScene(_d->doc.scene());
    }
}