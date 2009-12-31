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
#include "BrushCommand.h"

#include "Stable.h"
#include <QtOpenGL>
#include <QMouseEvent>
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "IDocument.h"
#include "IConfigContainer.h"
#include "IObject3D.h"
#include "DocumentView.h"
#include "Camera.h"
#include "BrushProperties.h"

QImage BrushCommand::m_cursorImage;

BrushCommand::BrushCommand(ICommand* parent)
	: CommandBase("Brush", parent),
	m_object(NULL),
	m_radius(0.5),
	m_depth( 0.1),
	m_action(Push),
	m_propertiesWindow(NULL),
	m_undoCalled(false),
	m_direction(-1)
{
	if (getOptionsWidget())
	{
		m_propertiesWindow->setBrushRadius(m_radius);
		m_propertiesWindow->setBrushStrength(m_depth);
		m_propertiesWindow->setBrushAction(Push);
	}
	if (m_cursorImage.isNull())
	{
		m_cursorImage.load("/Users/rcabral/images/test.png");
	}
}

BrushCommand::BrushCommand(const BrushCommand& cpy)
	: CommandBase(cpy),
	m_object(cpy.m_object),
	m_radius(cpy.m_radius),
	m_depth(cpy.m_depth),
	m_action(cpy.m_action),
	m_propertiesWindow(cpy.m_propertiesWindow),
	m_undoCalled(cpy.m_undoCalled)
{
	qDebug() << "BrushCommand::BrushCommand(cpy)";
	if (getOptionsWidget())
	{
		m_propertiesWindow->setBrushRadius(m_radius);
		m_propertiesWindow->setBrushStrength(m_depth);
		m_propertiesWindow->setBrushAction(cpy.m_action);
	}
	if (m_cursorImage.isNull())
	{
		m_cursorImage.load("/Users/rcabral/images/test.png");
	}
}

BrushCommand::~BrushCommand()
{
	qDebug() << "BrushCommand::~BrushCommand()";
}

ICommand* BrushCommand::clone() const
{
	qDebug() << "BrushCommand::clone()";
	return new BrushCommand(*this);
}

QWidget* BrushCommand::getOptionsWidget()
{
	if (m_propertiesWindow == NULL)
		m_propertiesWindow = new BrushProperties(NULL);
	return m_propertiesWindow;
}

void BrushCommand::activate(bool active)
{
	DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
	if (active)
	{
		view->setCursorImage(m_cursorImage);
		view->set3DCursorShape(GlView::Image);
	}
	else
	{
		view->set3DCursorShape(GlView::None);
	}
	CommandBase::activate(active);
}

/**
 * Undo the command. The undo information is in the previous state hash. It's
 * the same hash table used for the redo command.
 */
void BrushCommand::undo()
{
	qDebug() << "BrushCommand::undo()";
	if (m_object)
	{
		QHash<int, Point3D> hash = m_previousState[m_object];
		QHash<int, Point3D>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Point3D v = m_object->getVertex(it.key());
			m_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			m_object->adjustPointNormal(it.key());
		}
		m_object->setChanged(true);
		
		DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
		if (view)
			view->updateView();
	}
	m_undoCalled = true;
}

/**
 * Redo the command. The redo information is in the previous state hash. It's
 * the same hash table used for the undo command. So the redo should be used
 * only after the undo command, otherwise, the results would not be correct.
 */
void BrushCommand::redo()
{
	qDebug() << "BrushCommand::redo()";
	if (m_object && m_undoCalled)
	{
		QHash<int, Point3D> hash = m_previousState[m_object];
		QHash<int, Point3D>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Point3D v = m_object->getVertex(it.key());
			m_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			m_object->adjustPointNormal(it.key());
		}
		m_object->setChanged(true);
		
		DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
		if (view)
			view->updateView();
	}
}

void BrushCommand::mouseMoveEvent(QMouseEvent* e)
{
	if (e->buttons() == Qt::NoButton)
	{
		CommandBase::mouseMoveEvent(e);
		return;
	}

    if (m_selectedObjects.count() > 0)
    {
        GLdouble x = 0.0, y = 0.0,  z = 0.0;
        GLfloat wz = 0.0f;

        // Use the same z depth to move in the plane parallel to the screen.
        wz = m_currentWinPoint.z();
        gluUnProject(e->x(), m_viewPort[3] - e->y(), wz,
            m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

        Point3D currPoint = Point3D(x, y, z);
        Point3D currWinPoint = Point3D(e->x(), e->y(), wz);

        // Set the direction of the displacement
        m_direction = m_action == Push ? -1 : 1;

        applyOperation();

        m_currentPoint = currPoint;
        m_currentWinPoint = currWinPoint;

        DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
        m_selectedObjects.clear();
        m_selectedObjects = view->getSelectedObjects( e->pos().x(), e->pos().y());
        if (m_selectedObjects.count() > 0)
        {
            m_vertexSelected.clear();
            selectObject();
        }
		if (view)
			view->updateView();
    }
    else
    {
        CommandBase::mouseMoveEvent(e);
    }
}

void BrushCommand::mousePressEvent(QMouseEvent* e)
{
    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

	m_vertexSelected.clear();
	m_selectedObjects.clear();
	m_previousState.clear();
    m_radius = m_propertiesWindow->getBrushRadius();
    m_action = m_propertiesWindow->getBrushAction();
    m_depth = m_propertiesWindow->getBrushStrength();

    m_selectedObjects = view->getSelectedObjects( e->pos().x(), e->pos().y());

    if (m_selectedObjects.count() > 0)
    {
        GLdouble x, y, z;
        GLfloat wz = 0.0f;

        glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
        glGetIntegerv(GL_VIEWPORT, m_viewPort);

        glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
        gluUnProject(e->x(), m_viewPort[3] - e->y(), wz,
            m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

        m_initialWinPoint = Point3D(e->x(), e->y(), wz);
        m_currentWinPoint = m_initialWinPoint;
        m_intialPoint = Point3D(x, y, z);
        m_currentPoint = m_intialPoint;

		selectObject();
    }
    else
    {
        CommandBase::mousePressEvent(e);
    }

	if (view)
		view->updateView();
}

void BrushCommand::mouseReleaseEvent(QMouseEvent* e)
{
    m_vertexSelected.clear();
    if (m_selectedObjects.count() > 0)
    {
        m_selectedObjects.clear();
		m_object->setChanged(true);
        emit executed();
    }
    else
    {
        CommandBase::mouseReleaseEvent(e);
    }
	DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
	if (view)
		view->updateView();
}

void BrushCommand::applyOperation()
{
	GLdouble winX, winY, winZ;
	Point3D wv;
	if (m_vertexSelected.size() > 0)
	{
		Normal& n = m_object->getNormalAtPoint(m_vertexSelected[m_vertexSelected.count()/2]);
		for (int i = 0; i < m_vertexSelected.size(); i++)
		{
			
			Point3D& v = m_object->getVertex(m_vertexSelected[i]);
			gluProject(v.x(), v.y(), v.z(),
					   m_modelMatrix, m_projMatrix, m_viewPort,
					   &winX, &winY, &winZ);
			//wv.setPoint(winX, winY, winZ);
			wv.x() = winX;
			wv.y() = winY;
			wv.z() = winZ;
			//if ( n != Normal::null())
			{
				//float factor =  (m_radius - (m_currentWinPoint - wv).length()) / m_radius;
				float factor =  (m_radius - (m_currentPoint - v).norm()) / m_radius;
				factor = factor * m_depth * m_direction;
				Point3D disp = n * factor;
				v = v + disp;
				m_object->adjustPointNormal(m_vertexSelected[i]);
			}
		}
		m_object->setChanged(true);
	}
}

void BrushCommand::selectObject()
{
	qDebug() << "BrushCommand::selectObject()";
	DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    if (!view)
        return;
    int recordCount = m_selectedObjects.count();
    qDebug() << "recordCount =" << recordCount;
    for (int i = 0; i < recordCount; i++)
    {
		m_object = m_selectedObjects[i];
		if (m_object)
		{
//			m_vertexSelected = view->getSelectedVertices(m_currentWinPoint.getX(), 
//														m_currentWinPoint.getY(),
//														m_radius, m_radius);
			m_vertexSelected = m_object->getPointsInRadius(m_currentPoint, m_radius);
			int counter = m_vertexSelected.size();
			//qDebug() << "currentPoint: " << qPrintable(m_currentPoint.toString()) << " points selected: " << counter;
			for (int j = 0; j < counter; j++)
			{
				int index = m_vertexSelected[j];
				if (!m_previousState[m_object].contains(m_vertexSelected[j]))
				{
					//m_object->getPointList().at(index).color = QColor(255, 0, 0);
					m_previousState[m_object].insert(index, m_object->getVertex(index));
				}
			}
		}
		else
		{
			qDebug() << "m_object is null";
		}
    }
}


