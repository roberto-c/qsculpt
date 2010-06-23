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
#include "ISurface.h"
#include "DocumentView.h"
#include "Camera.h"
#include "BrushProperties.h"

QImage BrushCommand::_cursorImage;

BrushCommand::BrushCommand(ICommand* parent)
	: CommandBase("Brush", parent),
	_object(NULL),
	_radius(0.5),
	_depth( 0.1),
	_action(Push),
	_propertiesWindow(NULL),
	_undoCalled(false),
	_direction(-1)
{
	if (getOptionsWidget())
	{
		_propertiesWindow->setBrushRadius(_radius);
		_propertiesWindow->setBrushStrength(_depth);
		_propertiesWindow->setBrushAction(Push);
	}
	if (_cursorImage.isNull())
	{
		_cursorImage.load("/Users/rcabral/images/test.png");
	}
}

BrushCommand::BrushCommand(const BrushCommand& cpy)
	: CommandBase(cpy),
	_object(cpy._object),
	_radius(cpy._radius),
	_depth(cpy._depth),
	_action(cpy._action),
	_propertiesWindow(cpy._propertiesWindow),
	_undoCalled(cpy._undoCalled)
{
	qDebug() << "BrushCommand::BrushCommand(cpy)";
	if (getOptionsWidget())
	{
		_propertiesWindow->setBrushRadius(_radius);
		_propertiesWindow->setBrushStrength(_depth);
		_propertiesWindow->setBrushAction(cpy._action);
	}
	if (_cursorImage.isNull())
	{
		_cursorImage.load("/Users/rcabral/images/test.png");
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
	if (_propertiesWindow == NULL)
		_propertiesWindow = new BrushProperties(NULL);
	return _propertiesWindow;
}

void BrushCommand::activate(bool active)
{
	DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
	if (active)
	{
		view->setCursorImage(_cursorImage);
		view->set3DCursorShape(GlCanvas::Image);
	}
	else
	{
		view->set3DCursorShape(GlCanvas::None);
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
	if (_object)
	{
		QHash<int, Point3> hash = _previousState[_object];
		QHash<int, Point3>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Point3 v = _object->getVertex(it.key());
			_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			_object->adjustPointNormal(it.key());
		}
		_object->setChanged(true);
		
		DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
		if (view)
			view->updateView();
	}
	_undoCalled = true;
}

/**
 * Redo the command. The redo information is in the previous state hash. It's
 * the same hash table used for the undo command. So the redo should be used
 * only after the undo command, otherwise, the results would not be correct.
 */
void BrushCommand::redo()
{
	qDebug() << "BrushCommand::redo()";
	if (_object && _undoCalled)
	{
		QHash<int, Point3> hash = _previousState[_object];
		QHash<int, Point3>::iterator it, end = hash.end();
		for (it = hash.begin(); it != end; ++it)
		{
			Point3 v = _object->getVertex(it.key());
			_object->getVertex(it.key()) = it.value();
			it.value() = v;
		}
		for (it = hash.begin(); it != end; ++it)
		{
			_object->adjustPointNormal(it.key());
		}
		_object->setChanged(true);
		
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

    if (_selectedObjects.count() > 0)
    {
        GLdouble x = 0.0, y = 0.0,  z = 0.0;
        GLfloat wz = 0.0f;

        // Use the same z depth to move in the plane parallel to the screen.
        wz = _currentWinPoint.z();
        gluUnProject(e->x(), _viewPort[3] - e->y(), wz,
            _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);

        Point3 currPoint = Point3(x, y, z);
        Point3 currWinPoint = Point3(e->x(), e->y(), wz);

        // Set the direction of the displacement
        _direction = _action == Push ? -1 : 1;

        applyOperation();

        _currentPoint = currPoint;
        _currentWinPoint = currWinPoint;

        DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
        _selectedObjects.clear();
        _selectedObjects = view->getSelectedObjects( e->pos().x(), e->pos().y());
        if (_selectedObjects.count() > 0)
        {
            _vertexSelected.clear();
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

	_vertexSelected.clear();
	_selectedObjects.clear();
	_previousState.clear();
    _radius = _propertiesWindow->getBrushRadius();
    _action = _propertiesWindow->getBrushAction();
    _depth = _propertiesWindow->getBrushStrength();

    _selectedObjects = view->getSelectedObjects( e->pos().x(), e->pos().y());

    if (_selectedObjects.count() > 0)
    {
        GLdouble x, y, z;
        GLfloat wz = 0.0f;

        glGetDoublev(GL_MODELVIEW_MATRIX, _modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, _projMatrix);
        glGetIntegerv(GL_VIEWPORT, _viewPort);

        glReadPixels(e->x(), _viewPort[3] - e->y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
        gluUnProject(e->x(), _viewPort[3] - e->y(), wz,
            _modelMatrix, _projMatrix, _viewPort, &x, &y, &z);

        _initialWinPoint = Point3(e->x(), e->y(), wz);
        _currentWinPoint = _initialWinPoint;
        _intialPoint = Point3(x, y, z);
        _currentPoint = _intialPoint;

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
    _vertexSelected.clear();
    if (_selectedObjects.count() > 0)
    {
        _selectedObjects.clear();
		_object->setChanged(true);
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
	Point3 wv;
	if (_vertexSelected.size() > 0)
	{
		//Vector3& n = _object->getNormalAtPoint(_vertexSelected[_vertexSelected.count()/2]);
        Vector3 n;
		for (int i = 0; i < _vertexSelected.size(); i++)
		{
			
			Point3& v = _object->getVertex(_vertexSelected[i]);
			gluProject(v.x(), v.y(), v.z(),
					   _modelMatrix, _projMatrix, _viewPort,
					   &winX, &winY, &winZ);
			//wv.setPoint(winX, winY, winZ);
			wv.x() = winX;
			wv.y() = winY;
			wv.z() = winZ;
			//if ( n != Normal::null())
			{
				//float factor =  (_radius - (_currentWinPoint - wv).length()) / _radius;
				float factor =  (_radius - (_currentPoint - v).norm()) / _radius;
				factor = factor * _depth * _direction;
				Point3 disp = n * factor;
				v = v + disp;
				_object->adjustPointNormal(_vertexSelected[i]);
			}
		}
		_object->setChanged(true);
	}
}

void BrushCommand::selectObject()
{
	qDebug() << "BrushCommand::selectObject()";
	DocumentView* view = g_pApp->getMainWindow()->getCurrentView();

    if (!view)
        return;
    int recordCount = _selectedObjects.count();
    qDebug() << "recordCount =" << recordCount;
    for (int i = 0; i < recordCount; i++)
    {
		_object = _selectedObjects[i];
		if (_object)
		{
//			_vertexSelected = view->getSelectedVertices(_currentWinPoint.getX(), 
//														_currentWinPoint.getY(),
//														_radius, _radius);
			_vertexSelected = _object->getPointsInRadius(_currentPoint, _radius);
			int counter = _vertexSelected.size();
			//qDebug() << "currentPoint: " << qPrintable(_currentPoint.toString()) << " points selected: " << counter;
			for (int j = 0; j < counter; j++)
			{
				int index = _vertexSelected[j];
				if (!_previousState[_object].contains(_vertexSelected[j]))
				{
					//_object->getPointList().at(index).color = QColor(255, 0, 0);
					_previousState[_object].insert(index, _object->getVertex(index));
				}
			}
		}
		else
		{
			qDebug() << "_object is null";
		}
    }
}


