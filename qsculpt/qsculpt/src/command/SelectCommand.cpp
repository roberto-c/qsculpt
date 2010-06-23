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

QPointer<TransformWidget> SelectCommand::_objectProperties = NULL;

SelectCommand::SelectCommand(ICommand* parent)
: CommandBase("Select", parent),
_boxSelection(true),
_drawBox(false)
{
	if(getOptionsWidget())
	{
		// TODO: initialize object properties window
	}
}

SelectCommand::SelectCommand(const SelectCommand& cpy)
: CommandBase(cpy),
_boxSelection(cpy._boxSelection),
_drawBox(false)
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

void SelectCommand::mouseMoveEvent(QMouseEvent* e)
{
    if (_boxSelection) 
    {
        _endPoint = Point3(e->pos().x(), e->pos().y(), 0.5f);
        _drawBox = true;
    } 
    else 
    {
        if (_objectsSelected.isEmpty())
            CommandBase::mouseMoveEvent(e);
    }
}

void SelectCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    if (_boxSelection) 
    {
        _startPoint = Point3(e->pos().x(), e->pos().y(), 0.5f);
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
            CommandBase::mousePressEvent(e);
        }
    }
}

void SelectCommand::mouseReleaseEvent(QMouseEvent* e)
{
    if (_boxSelection) 
    {
        _endPoint = Point3(e->pos().x(), e->pos().y(), 0.5f);
        _drawBox = false;
        emit executed();
    } 
    else 
    {
        if (_objectsSelected.count() > 0)
            emit executed();
        else
            CommandBase::mouseReleaseEvent(e);
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
        c->drawRect(_startPoint, _endPoint);
        c->enable(GL_LIGHTING);
        c->disable(GL_BLEND);
        c->enable(GL_DEPTH_TEST);
    }
}