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
#include "selectcommand.h"

#include <QtOpenGL>
#include <QMouseEvent>
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "idocument.h"
#include "iconfigcontainer.h"
#include "iobject3d.h"
#include "documentview.h"
#include "camera.h"

SelectCommand::SelectCommand()
    : CommandBase()
{
}


SelectCommand::~SelectCommand()
{

}


void SelectCommand::mouseMoveEvent(QMouseEvent* e)
{
    if (m_record.isEmpty())
        CommandBase::mouseMoveEvent(e);
}

void SelectCommand::mousePressEvent(QMouseEvent* e)
{    
    DocumentView* view = SPAPP->getMainWindow()->getCurrentView();
    
    m_record = view->getPickRecords( e->pos().x(), e->pos().y());
    
    if (m_record.size() > 0)
    {
        selectObject();
    }
    else
    {        
        CommandBase::mousePressEvent(e);
    }
}

void SelectCommand::mouseReleaseEvent(QMouseEvent* e)
{   
    if (m_record.size() > 0)
        emit executed();
    else
        CommandBase::mouseReleaseEvent(e);
}

void SelectCommand::selectObject()
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
            IObject3D* obj = doc->getObject(m_record[i].stackContents - 1);
            if (obj)
            {
                obj->showBoundingBox( !obj->getShowBoundingBox() );
            }
        }
    }
}

