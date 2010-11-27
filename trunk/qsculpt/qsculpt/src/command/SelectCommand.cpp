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
#include "SelectCommand.h"
#include <QtOpenGL>
#include <QMouseEvent>
#include <QPointer>
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "IDocument.h"
#include "IConfigContainer.h"
#include "ISurface.h"
#include "DocumentView.h"
#include "Camera.h"
#include "TransformWidget.h"
#include "Aabb.h"

QPointer<TransformWidget> SelectCommand::_objectProperties = NULL;

SelectCommand::SelectCommand(ICommand* parent)
    : CommandBase("Select", parent),
    _boxSelection(true),
    _drawBox(false),
    _selectionType(ST_Surface)
{
    _configContainer->setInt(SELECT_TYPE, ST_Surface);

    if(getOptionsWidget())
    {
        // TODO: initialize object properties window
    }
}

SelectCommand::SelectCommand(const SelectCommand& cpy)
    : CommandBase(cpy),
    _boxSelection(cpy._boxSelection),
    _drawBox(false),
    _selectionType(cpy._selectionType)
{
    if(getOptionsWidget())
    {
        // TODO: initialize object properties window
    }
}

SelectCommand::~SelectCommand()
{

}

ICommand* SelectCommand::clone() const
{
    return new SelectCommand(*this);
}

QWidget* SelectCommand::getOptionsWidget()
{
    if (_objectProperties == NULL)
        _objectProperties = new TransformWidget(NULL);
    return _objectProperties;
}

void SelectCommand::execute()
{
    bool ok = true;
    uint iid = 0;
    // If the config container has a numeric key, then we assume it was
    // called by the console.
    qDebug() << "SelectCommand execute";
    if (_configContainer->containsKey("0"))
    {
        QString cmdName = _configContainer->getString("0");
        QString cmdArg1 = _configContainer->getString("1");
        if (cmdName == "select")
        {
            if (cmdArg1 == "object")
            {
                iid = _configContainer->getString("2").toUInt(&ok);
                if (!ok) {
                    qDebug() << "error in parameter";
                    return;
                }
                
                DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
                assert(view);
                ISurface* s = view->getDocument()->getObject(iid);
                s->setSelected(true);
                s->setChanged(true);
                view->updateView();
                emit executed();
            }
        }
    }
}

void SelectCommand::mouseMoveEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    if (_boxSelection) 
    {
        _endPointWin = Point3(e->pos().x(), view->getCanvas()->height() - e->pos().y(), 1.0f);
        view->getCanvas()->mapScreenCoordsToWorldCoords(_endPointWin, _endPoint);
        _drawBox = true;
        select();
    } 
    else 
    {
        if (_objectsSelected.isEmpty())
        {
            //    CommandBase::mouseMoveEvent(e);
        }
    }
}

void SelectCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    unselectAll();

    // Get the config options
    _selectionType = (SelectionType)_configContainer->getInt(SELECT_TYPE);

    if (_boxSelection) 
    {
        _startPointWin = Point3(e->pos().x(), view->getCanvas()->height() - e->pos().y(), 0.0f);
        view->getCanvas()->mapScreenCoordsToWorldCoords(_startPointWin, _startPoint);
    } 
    else 
    {
        _objectsSelected = view->getSelectedObjects( e->pos().x(), e->pos().y());

        if (_objectsSelected.count() > 0)
        {
            for (int i = 0; i < _objectsSelected.count(); ++i)
            {
                _objectsSelected[i]->setSelected(!_objectsSelected[i]->isSelected());
            }
        }
        else
        {
            //    CommandBase::mousePressEvent(e);
        }
    }
}

void SelectCommand::mouseReleaseEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    if (_boxSelection) 
    {
        _endPointWin = Point3(e->pos().x(), view->getCanvas()->height() - e->pos().y(), 1.0f);
        view->getCanvas()->mapScreenCoordsToWorldCoords(_endPointWin, _endPoint);
        _drawBox = false;
        select();
        emit executed();
    } 
    else 
    {
        if (_objectsSelected.count() > 0)
            emit executed();
        else
        {
            //    CommandBase::mouseReleaseEvent(e);
        }
    }
}

void SelectCommand::paintGL(GlCanvas *c)
{
    if (_drawBox) {
        c->disable(GL_LIGHTING);
        c->enable(GL_BLEND);
        c->disable(GL_DEPTH_TEST);
        c->setPen(QPen(QColor(49, 122, 255, 255)));
        c->setBrush(QBrush(QColor(49, 122, 255, 100)));
        c->drawRect(_startPointWin, _endPointWin);
        c->enable(GL_LIGHTING);
        c->disable(GL_BLEND);
        c->enable(GL_DEPTH_TEST);
    }
}

void SelectCommand::select()
{
    switch(_selectionType) {
    case ST_Vertex:
        selectVertices();
        break;
    case ST_Surface:
        selectSurface();
        break;
    case ST_Face:
        selectFaces();
        break;
    case ST_Edge:
        selectEdges();
        break;
    }
}

void SelectCommand::selectVertices()
{
    using namespace geometry;
    using namespace std;

    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    assert(view);

    AABB box;
    box.extend(_startPointWin).extend(_endPointWin);

    Point3 p;
    ISurface *surface = NULL;
    if (view->getDocument()->getObjectsCount() > 0) {
        surface = view->getDocument()->getObject(0);
    }
    if(surface) {
        Iterator<Vertex> it = surface->vertexIterator();
        while(it.hasNext()) {
            Vertex* v = &it.next();
            view->getCanvas()->mapWorldCoordsToScreenCoords(v->position(), p);
            //qDebug() << toString(v->position()) << toString(p);
            v->removeFlag(VF_Selected);
            if (box.contains(p)) {
                v->addFlag(VF_Selected);
            }
        }
        surface->setChanged(true);
    }
}

void SelectCommand::selectSurface()
{
    using namespace geometry;
    using namespace std;

    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    assert(view);

    AABB box;
    box.extend(_startPointWin).extend(_endPointWin);

    Point3 p;
    ISurface *surface = NULL;
    if (view->getDocument()->getObjectsCount() > 0) {
        surface = view->getDocument()->getObject(0);
    }
    if(surface) {
        surface->setSelected(false);
        Iterator<Vertex> it = surface->vertexIterator();
        while(it.hasNext()) {
            Vertex* v = &it.next();
            view->getCanvas()->mapWorldCoordsToScreenCoords(v->position(), p);
            if (box.contains(p)) {
                surface->setSelected(true);
            }
        }
        surface->setChanged(true);
    }
}

void SelectCommand::selectFaces()
{
    using namespace geometry;
    using namespace std;

    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    assert(view);

    AABB box;
    box.extend(_startPointWin).extend(_endPointWin);

    Point3 p;
    ISurface *surface = NULL;
    if (view->getDocument()->getObjectsCount() > 0) {
        surface = view->getDocument()->getObject(0);
    }
    if(surface) {
        surface->setSelected(false);
        Iterator<Face> it = surface->faceIterator();
        while(it.hasNext()) {
            Face* f = &it.next();
            f->removeFlag(FF_Selected);
            Iterator<Vertex> vtxIt = f->vertexIterator();
            while(vtxIt.hasNext()) {
                Vertex* v = &vtxIt.next();
                view->getCanvas()->mapWorldCoordsToScreenCoords(v->position(), p);
                //qDebug() << toString(v->position()) << toString(p);
                if (box.contains(p)) {
                    f->addFlag(FF_Selected);
                    surface->setSelected(true);
                }
            }
        }
        surface->setChanged(true);
    }
}

void SelectCommand::selectEdges()
{

}

void SelectCommand::unselectAll()
{
    using namespace geometry;
    using namespace std;

    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    assert(view);
    ISurface *surface = NULL;
    if (view->getDocument()->getObjectsCount() > 0) {
        surface = view->getDocument()->getObject(0);
    }
    if(surface) {
        surface->setSelected(false);
        Iterator<Face> it = surface->faceIterator();
        while(it.hasNext()) {
            Face* f = &it.next();
            f->removeFlag(FF_Selected);
            Iterator<Vertex> vtxIt = f->vertexIterator();
            while(vtxIt.hasNext()) {
                Vertex* v = &vtxIt.next();
                v->removeFlag(VF_Selected);
            }
        }
        surface->setChanged(true);
    }
}

