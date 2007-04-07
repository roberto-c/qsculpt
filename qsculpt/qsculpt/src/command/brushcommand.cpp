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


BrushCommand::BrushCommand()
    : CommandBase(),
    m_object(NULL),
    m_radius(0.5),
    m_depth( 0.1),
    m_action(Push),
    m_propertiesWindow(NULL)
{
    m_propertiesWindow = new BrushProperties(NULL);
    m_propertiesWindow->setBrushRadius(m_radius);
    m_propertiesWindow->setBrushStrength(m_depth);
    m_propertiesWindow->setBrushAction(Push);
}


BrushCommand::~BrushCommand()
{

}

void BrushCommand::activate(bool active)
{
    CommandBase::activate(active);

    if (active)
    {
        if (m_propertiesWindow)
            SPAPP->getMainWindow()->setOptionsWidget(m_propertiesWindow);
        else
            qDebug("Properties window is null");
    }
    else
    {
        if (m_propertiesWindow)
            SPAPP->getMainWindow()->setOptionsWidget(NULL);
    }
}

void BrushCommand::mouseMoveEvent(QMouseEvent* e)
{
    
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
            Point3D newPos = currPoint - m_currentPoint;
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
    }
    else
    {
        CommandBase::mouseMoveEvent(e);
    }
}

void BrushCommand::mousePressEvent(QMouseEvent* e)
{   
    DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
    
    m_radius = m_propertiesWindow->getBrushRadius();
    m_action = m_propertiesWindow->getBrushAction();
    m_depth = m_propertiesWindow->getBrushStrength();
    
    m_record = view->getPickRecords( e->pos().x(), e->pos().y());
    
    if (m_record.size() > 0)
    {
        selectObject();
    
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
        
        glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
        glGetIntegerv(GL_VIEWPORT, m_viewPort);
    }
    else
    {
        CommandBase::mousePressEvent(e);
    }
}

void BrushCommand::mouseReleaseEvent(QMouseEvent* e)
{
    m_vertexSelected.clear();
    if (m_record.size() > 0)
    {
        m_record.clear();
        emit executed();
    }
    else
    {
        CommandBase::mouseReleaseEvent(e);
    }
}

void BrushCommand::selectObject()
{
    const IDocument* doc = SPAPP->getMainWindow()->getCurrentDocument();
    
    if (!doc)
        return;
    
    int docObjectCount = doc->getObjectsCount();
    int recordCount = m_record.size();
    for (int i = 0; i < recordCount; i++)
    {
        if (m_record[i].stackContents > 0 && m_record[i].stackContents - 1 < docObjectCount)
        {
            m_object = doc->getObject(m_record[i].stackContents - 1);
            if (m_object)
            {
                SPAPP->getMainWindow()->getCurrentView()->set3DCursorShape(GlDisplay::Cross);
                SPAPP->getMainWindow()->getCurrentView()->setCursorPosition(m_intialPoint);
                m_vertexSelected = m_object->getPointsInRadius(m_currentPoint, m_radius);
            }
        }
    }
}

