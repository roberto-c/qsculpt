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
#include "documentview.h"
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>

#include "sphere.h"
#include "box.h"
#include "idocument.h"
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "icommand.h"
#include "gldisplay.h"

DocumentView::DocumentView( QWidget *parent )
        : QWidget( parent ),
        m_document(NULL),
        m_viewPerspective(NULL),
        m_drawingMode(NULL)
{
    createWidgets();
}

DocumentView::~DocumentView() {}

void DocumentView::createWidgets()
{
    QGridLayout* layout = new QGridLayout(this);
    m_display = new GlDisplay(this);
    
    QLabel* label = new QLabel("View", this);    
    m_viewPerspective = new QComboBox(this);
    label->setBuddy(m_viewPerspective);
    
    layout->addWidget(m_display, 0,0, 1, 5);
    layout->addWidget(label, 1, 0);
    layout->addWidget(m_viewPerspective, 1, 1);
    
    label = new QLabel("Drawing Mode", this);
    m_drawingMode = new QComboBox(this);
    label->setBuddy(m_drawingMode);
    
    layout->addWidget(label, 1, 2);
    layout->addWidget(m_drawingMode, 1, 3);
    layout->addItem(new QSpacerItem(100, 0), 1, 4);
    layout->setColumnStretch(0,0);
    layout->setColumnStretch(1,0);
    layout->setColumnStretch(2,0);
    layout->setColumnStretch(3,0);
    layout->setColumnStretch(4,4);
    
    m_viewPerspective->addItem("Front", GlDisplay::Front);
    m_viewPerspective->addItem("Back", GlDisplay::Back);
    m_viewPerspective->addItem("Top", GlDisplay::Top);
    m_viewPerspective->addItem("Bottom", GlDisplay::Bottom);
    m_viewPerspective->addItem("Left", GlDisplay::Left);
    m_viewPerspective->addItem("Right", GlDisplay::Right);
    m_viewPerspective->addItem("Perspective", GlDisplay::Perspective);
    m_viewPerspective->setCurrentIndex(6);
    m_display->setPerspectiveView( GlDisplay::Perspective );
    
    m_drawingMode->addItem("Points", Points);
    m_drawingMode->addItem("Wireframe", Wireframe);
    m_drawingMode->addItem("Flat", Flat);
    m_drawingMode->addItem("Smooth", Smooth);
    m_drawingMode->setCurrentIndex(2);
    m_display->setDrawingMode( Flat );
    
    connect(m_viewPerspective, SIGNAL(currentIndexChanged(int)), this, SLOT(viewPerspectiveChanged(int)));
    connect(m_drawingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(drawingModeChanged(int)));
    
}

void DocumentView::setGridVisible( bool value)
{
    m_display->setGridVisible( value );
}

bool DocumentView::isGridVisible()
{
    return m_display->isGridVisible();
}

void DocumentView::updateView()
{
    m_display->updateGL();
}

void DocumentView::viewPerspectiveChanged(int index)
{
    GlDisplay::PerspectiveType type;
    
    type = (GlDisplay::PerspectiveType)m_viewPerspective->itemData(index).toInt();
    m_display->setPerspectiveView( type );
    updateView();
}

void DocumentView::drawingModeChanged(int index)
{
    DrawingMode type;
    
    type = (DrawingMode)m_viewPerspective->itemData(index).toInt();
    m_display->setDrawingMode( type );
    updateView();
}

void DocumentView::setNormalsVisible(bool value)
{
    m_display->setNormalsVisible(value);
}

