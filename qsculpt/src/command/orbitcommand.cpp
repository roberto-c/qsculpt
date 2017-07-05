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
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/Document.h>
#include <PlastilinaCore/FlatRenderer.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/geometry/Sphere.h>
#include <PlastilinaCore/math/Utils.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/subdivision/Sphere.h>
#include <QtGui/QMouseEvent>
#include "DocumentView.h"
#include "Eigen/Geometry"
#include "GlView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

struct OrbitCommand::Impl
{
    Point3                           initial, endPoint, centerPoint;
    Vector3                          startVector, endVector;
    Vector3                          initialVector;
    float                            radius;
    float                            startAngle;
    float                            endAngle;
    bool                             draw;
    std::vector<SceneNode::weak_ptr> selectedObj;
    Document                         doc;
    IRenderer*                       renderer;
    Eigen::Quaternionf               rot;
    Eigen::Transform3f               t;
    geometry::Ray                    ray;
    geometry::Sphere                 sphere;
    Camera                           camera;
    std::vector<Eigen::Affine3f>     originalTransform;

    Impl()
        : initial(Point3())
        , radius(0)
        , startAngle(0)
        , endAngle(360)
        , draw(false)
        , renderer(new FlatRenderer)
    {
        qDebug() << "OrbitCommand::Impl constructed";
    }

    Impl(const Impl& cpy)
        : initial(cpy.initial)
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

    ~Impl() { qDebug() << "OrbitCommand::Impl destruyed"; }

    void setup();
};

void OrbitCommand::Impl::setup()
{
    SceneNode::shared_ptr root(new SceneNode);
	ISurface::shared_ptr surf = std::make_shared<Sphere>();
    SurfaceNode::shared_ptr surface =
        std::make_shared<SurfaceNode>(surf, "No name");
    root->add(surface);
    surface->surface()->setColor(Color(1.0, 1.0, 0.0, 0.2));

    //    SurfaceNode *surface = new SurfaceNode(new Box, root);
    //    surface->transform() *= Eigen::Translation<float,3>(1, 0, 0);
    //    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    //
    //    surface = new SurfaceNode(new Box, root);
    //    surface->transform() *= Eigen::Translation<float,3>(0, 1, 0);
    //    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    //
    //    surface = new SurfaceNode(new Box, root);
    //    surface->transform() *= Eigen::Translation<float,3>(0, 0, 1);
    //    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);

    root->transform() *= Eigen::Translation<float, 3>(0, 0, 0);
    auto ptr = doc.scene().lock();
    if (ptr)
        ptr->add(root);
}

OrbitCommand::OrbitCommand(ICommand* parent)
    : CommandBase(parent)
    , _d(new Impl)
{
    _d->setup();
}

OrbitCommand::OrbitCommand(const OrbitCommand& cpy)
    : CommandBase(cpy)
    , _d(new Impl(*cpy._d))
{
    _d->setup();
}

OrbitCommand::~OrbitCommand() {}

ICommand* OrbitCommand::clone() const { return new OrbitCommand(*this); }

QWidget* OrbitCommand::getOptionsWidget() { return NULL; }

void OrbitCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    _d->selectedObj = view->getDocument()->getSelectedObjects();

    if (_d->selectedObj.size() > 0)
    {
        _d->draw = true;

        _d->originalTransform.clear();

        auto obj = _d->selectedObj[0].lock();
        _d->originalTransform.push_back(obj->transform());

        _d->camera = *view->getCanvas()->getViewCamera();

        _d->camera.setViewport(0, 0, view->getCanvas()->width(),
                               view->getCanvas()->height());
        // float aspect = view->getCanvas()->width() /
        // view->getCanvas()->height(); _d->camera.setOrthoMatrix(-aspect,
        // aspect, -1, 1, -1, 1); _d->camera.setTargetPoint( Point3( 0, 0, 0)
        //); _d->camera.setOrientationVector(Point3( 0, 1, 0) );
        //_d->camera.setPosition( Point3( 0, 0, 1));

        _d->draw        = true;
        _d->initial.x() = view->getCanvas()->width() / 2;
        _d->initial.y() = view->getCanvas()->height() / 2;

        _d->centerPoint =
            _d->camera.eyeToWorld(view->getCanvas()->width() / 2,
                                  view->getCanvas()->height() / 2, 0.5f);

        Vector3 v1 = _d->camera.eyeToWorld(
            e->x(), view->getCanvas()->height() - e->y(), 0.f);

        Vector3 v2 = _d->camera.eyeToWorld(
            e->x(), view->getCanvas()->height() - e->y(), 0.8f);
        _d->ray.origin()    = v1;
        _d->ray.direction() = v2 - v1;
        _d->ray.direction().normalize();
        if (_d->ray.intersect(_d->sphere, &_d->startVector) >= 0)
        {
            _d->startVector.normalize();
            _d->initialVector = _d->startVector;
        }
    }
}

void OrbitCommand::mouseReleaseEvent(QMouseEvent* e)
{
    if (_d->selectedObj.size() > 0)
    {
    }
    else
    {
        //        Iterator<SceneNode> it = _d->grid.scene()->iterator();
        //        while (it.hasNext()) {
        //            SceneNode* node = &it.next();
        //            node->transform() = _d->t;
        //        }
    }

    _d->draw = false;
}

void OrbitCommand::mouseMoveEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    if (_d->selectedObj.size() > 0)
    {
        Vector3 v1 = _d->camera.eyeToWorld(
            e->x(), view->getCanvas()->height() - e->y(), 0.f);

        Vector3 v2 = _d->camera.eyeToWorld(
            e->x(), view->getCanvas()->height() - e->y(), 0.8f);
        _d->ray.origin()    = v1;
        _d->ray.direction() = v2 - v1;
        _d->ray.direction().normalize();
        // qDebug() << "o = " << toString(_d->ray.origin()) << " d = " <<
        // toString(_d->ray.direction());
        if (_d->ray.intersect(_d->sphere, &_d->endVector) < 0)
        {
            qDebug() << "Intersection failed";
            return;
        }

        _d->endVector.normalize();
        // qDebug() << "End " << toString(_d->endVector);
        // if end vector and start vector are equal, exit as there is no
        // rotation
        if (_d->startVector == _d->endVector)
        {
            return;
        }
        Vector3 axis = _d->startVector.cross(_d->endVector);
        axis.normalize();
        float dot = _d->startVector.dot(_d->endVector);
        if (dot != dot)
        {
            return;
        }
        auto obj       = _d->selectedObj[0].lock();
        auto parentptr = obj->parent().lock();
        if (!parentptr)
        {
            qDebug() << "Failed to obtain pointer";
            return;
        }
        axis        = parentptr->transform() * axis;
        dot         = math::clamp(dot, -1.f, 1.f);
        float angle = acos(dot);
        std::swap(_d->startVector, _d->endVector);

        Eigen::AngleAxisf rot = Eigen::AngleAxisf(angle, axis);
        qDebug() << "Vector: " << toString(axis).c_str() << " @ " << angle;
        auto ptr = _d->doc.scene().lock();
        if (ptr)
        {
            Iterator<SceneNode> it = ptr->iterator();
            while (it.hasNext())
            {
                auto node = it.next();
                node->transform().rotate(rot);
            }
        }
        obj->setTransform(obj->transform().rotate(rot));
    }
}

void OrbitCommand::paintGL(GlCanvas* c)
{
    static QBrush brush(QColor(0, 120, 220, 180));
    static QPen   pen(QColor(0, 220, 220, 180));
    // DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    if (_d->draw)
    {
        //glClear(GL_DEPTH_BUFFER_BIT);
        auto ptr = _d->doc.scene().lock();
        if (ptr)
        {
            // c->drawScene(ptr.data());
        }
        //        c->disable(GL_LIGHTING);
        //        c->drawLine(Point3(0,0,0), _d->initialVector);
        //        c->drawLine(Point3(0,0,0), _d->endVector);
        //        c->enable(GL_LIGHTING);
    }
}