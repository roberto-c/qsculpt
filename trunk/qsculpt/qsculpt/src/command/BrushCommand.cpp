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
#include "brushcommand.h"

#include "stable.h"
#include <QtOpenGL>
#include <QMouseEvent>
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "idocument.h"
#include "iconfigcontainer.h"
#include "iobject3d.h"
#include "documentview.h"
#include "camera.h"
#include "brushproperties.h"

QImage BrushCommand::m_cursorImage;

BrushCommand::BrushCommand(ICommand* parent)
	: CommandBase("Brush", parent),
	m_object(NULL),
	m_radius(0.5),
	m_depth( 0.1),
	m_action(Push),
	m_propertiesWindow(NULL),
	m_undoCalled(false)
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
	DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
	if (active)
	{
		view->setCursorImage(m_cursorImage);
		view->set3DCursorShape(GlDisplay::Image);
	}
	else
	{
		view->set3DCursorShape(GlDisplay::None);
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
		QHash<int, Vertex> hash = m_previousState[m_object];
		QHash<int, Vertex>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Vertex v = m_object->getVertex(it.key());
			m_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			m_object->adjustPointNormal(it.key());
		}
		m_object->setChanged(true);
		
		DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
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
		QHash<int, Vertex> hash = m_previousState[m_object];
		QHash<int, Vertex>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Vertex v = m_object->getVertex(it.key());
			m_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			m_object->adjustPointNormal(it.key());
		}
		m_object->setChanged(true);
		
		DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
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

    if (m_record.size() > 0)
    {
        GLdouble x = 0.0,
            y = 0.0,
            z = 0.0;
        GLfloat wz = 0.0f;

        //glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
        // Use the same z depth to move in the plane parallel to the screen.
        wz = m_currentWinPoint.getZ();
        gluUnProject(e->x(), m_viewPort[3] - e->y(), wz,
            m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

        Point3D currPoint = Point3D(x, y, z);
        Point3D currWinPoint = Point3D(e->x(), m_viewPort[3] - e->y(), wz);

        // Set the direccion of the displacement
        int direction = m_action == Push ? -1 : 1;

        if (m_vertexSelected.size() > 0)
        {
            Normal& n = m_object->getNormalAtPoint(m_vertexSelected[0]);
            for (int i = 0; i < m_vertexSelected.size(); i++)
            {

                Vertex& v = m_object->getVertex(m_vertexSelected[i]);
                if ( n != Normal::null())
                {
                    float factor = (m_radius - (m_currentPoint - v).length()) / m_radius;
                    factor = factor * m_depth * direction;
                    Vertex disp = n * factor;
                    v = v + disp;
                    m_object->adjustPointNormal(m_vertexSelected[i]);
                }
            }
			m_object->setChanged(true);
        }

        m_currentPoint = currPoint;
        m_currentWinPoint = currWinPoint;

        DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
        m_record.clear();
        m_record = view->getPickRecords( e->pos().x(), e->pos().y());
        if (m_record.size() > 0)
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
    DocumentView* view = SPAPP->getMainWindow()->getCurrentView();

	m_vertexSelected.clear();
	m_record.clear();
	m_previousState.clear();
    m_radius = m_propertiesWindow->getBrushRadius();
    m_action = m_propertiesWindow->getBrushAction();
    m_depth = m_propertiesWindow->getBrushStrength();

    m_record = view->getPickRecords( e->pos().x(), e->pos().y());

    if (m_record.size() > 0)
    {
        GLdouble x, y, z;
        GLfloat wz = 0.0f;

        glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
        glGetIntegerv(GL_VIEWPORT, m_viewPort);

        glReadPixels(e->x(), m_viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
        gluUnProject(e->x(), m_viewPort[3] - e->y(), wz,
            m_modelMatrix, m_projMatrix, m_viewPort, &x, &y, &z);

        m_initialWinPoint = Point3D(e->x(), m_viewPort[3] - e->y(), wz);
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
    if (m_record.size() > 0)
    {
        m_record.clear();
		m_object->setChanged(true);
        emit executed();
    }
    else
    {
        CommandBase::mouseReleaseEvent(e);
    }
	DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
	if (view)
		view->updateView();
}

void BrushCommand::selectObject()
{
	qDebug() << "BrushCommand::selectObject()";
    const IDocument* doc = SPAPP->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;

    int docObjectCount = doc->getObjectsCount();
    int recordCount = m_record.size();
    qDebug() << "recordCount =" << recordCount;
    for (int i = 0; i < recordCount; i++)
    {
        if (m_record[i].stackContents > 0 && m_record[i].stackContents - 1 < docObjectCount)
        {
        	qDebug() << "record is between bounds" << recordCount;
            m_object = doc->getObject(m_record[i].stackContents - 1);
            if (m_object)
            {
                m_vertexSelected = m_object->getPointsInRadius(m_currentPoint, m_radius);
				int counter = m_vertexSelected.size();
				qDebug() << "currentPoint: " << qPrintable(m_currentPoint.toString()) << " points selected: " << counter;
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
        else
        {
        	qDebug() << "record is out of bounds: " << m_record[i].stackContents;
        }
    }
}


