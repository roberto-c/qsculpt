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
#include "IObject3D.h"
#include "DocumentView.h"
#include "Camera.h"
#include "TransformWidget.h"

QPointer<TransformWidget> SelectCommand::m_objectProperties = NULL;

SelectCommand::SelectCommand(ICommand* parent)
    : CommandBase("Select", parent)
{
	if(getOptionsWidget())
	{
		// TODO: initialize object properties window
	}
}

SelectCommand::SelectCommand(const SelectCommand& cpy)
: CommandBase(cpy)
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
	if (m_objectProperties == NULL)
		m_objectProperties = new TransformWidget(NULL);
	return m_objectProperties;
}

void SelectCommand::mouseMoveEvent(QMouseEvent* e)
{
    if (m_objectsSelected.isEmpty())
        CommandBase::mouseMoveEvent(e);
}

void SelectCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    m_objectsSelected = view->getSelectedObjects( e->pos().x(), e->pos().y());
	//QVector<int> verticesSelected = view->getSelectedVertices(e->pos().x(), e->pos().y(), 30, 30);

    if (m_objectsSelected.count() > 0)
    {
		for (int i = 0; i < m_objectsSelected.count(); ++i)
		{
			m_objectsSelected[i]->setSelected(!m_objectsSelected[i]->isSelected());
			//m_objectsSelected[i]->setSelectedPoints(verticesSelected);
		}
    }
    else
    {
        CommandBase::mousePressEvent(e);
    }
}

void SelectCommand::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_objectsSelected.count() > 0)
        emit executed();
    else
        CommandBase::mouseReleaseEvent(e);
}

