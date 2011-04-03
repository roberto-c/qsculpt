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
#include "orbitcommand.h"
#include "GlView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"
#include "Document.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Box.h"
#include "FlatRenderer.h"
#include "Eigen/Geometry"

struct OrbitCommand::Impl
{
    Point3  initial;
    Point3  startVector, endVector;
    float   radius;
    float   startAngle;
    float   endAngle;
    bool    draw;
    QList<ISurface*> selectedObj;
    Document doc;
    IRenderer *renderer;
    
    Impl() : 
    initial(Point3())
    , radius(0)
    , startAngle(0)
    , endAngle(360)
    , draw(false)
    , renderer(new FlatRenderer)
    {
    }
    
    Impl(const Impl& cpy) :
    initial(cpy.initial)
    , startVector(cpy.startVector)
    , endVector(cpy.endVector)
    , radius(cpy.radius)
    , startAngle(cpy.startAngle)
    , endAngle(cpy.endAngle)
    , draw(false)
    , selectedObj(cpy.selectedObj)
    , renderer(new FlatRenderer)
    {
    }
    
    void setup();
};

void OrbitCommand::Impl::setup()
{
    SurfaceNode *surface = new SurfaceNode(new Box);
    surface->transform() *= Eigen::Translation<float,3>(1, 0, 0);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    doc.scene()->appendRow(surface);
    
    surface = new SurfaceNode(new Box);
    surface->transform() *= Eigen::Translation<float,3>(0, 1, 0);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    doc.scene()->appendRow(surface);
    
    surface = new SurfaceNode(new Box);
    surface->transform() *= Eigen::Translation<float,3>(0, 0, 1);
    surface->transform() *= Eigen::AlignedScaling3f(0.2,0.2,0.2);
    doc.scene()->appendRow(surface);
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
    }
}

void OrbitCommand::mouseReleaseEvent(QMouseEvent *e)
{
    _d->draw = false;
}

void OrbitCommand::mouseMoveEvent(QMouseEvent *e)
{
    if (_d->selectedObj.size() == 0) return;
    
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    
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
}

void OrbitCommand::paintGL(GlCanvas *c)
{
    static QBrush brush(QColor(0, 120, 220, 180));
    static QPen pen(QColor(0, 220, 220, 180));
    
    if (_d->draw) {
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
//        qDebug() << _d->startAngle << " | " << _d->endAngle;
        
        c->drawLine(_d->initial, _d->initial + 100*(_d->startVector));
        c->drawLine(_d->initial, _d->initial + 100*(_d->endVector));
    }
    c->drawScene(_d->doc.scene());
}