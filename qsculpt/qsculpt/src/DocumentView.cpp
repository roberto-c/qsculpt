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
#include "DocumentView.h"
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>

#include "Sphere.h"
#include "Box.h"
#include "IDocument.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "ICommand.h"
#include "GlDisplay.h"

DocumentView::DocumentView( QWidget *_parent )
        : QWidget( _parent ),
        m_document(NULL),
        m_viewPerspective(NULL),
        m_drawingMode(NULL)
{
    createWidgets();
}

DocumentView::~DocumentView() {}

void DocumentView::createWidgets()
{
    QGridLayout* gridLayout = new QGridLayout(this);
	QHBoxLayout* hboxLayout = new QHBoxLayout();
	m_display = new GlDisplay(this);
	
	Q_CHECK_PTR(gridLayout);
	Q_CHECK_PTR(m_display);
	Q_CHECK_PTR(hboxLayout);

	// Set up gl display format
	QGLFormat format = m_display->format();
	format.setSwapInterval(1);
	format.setDepthBufferSize(32);
	m_display->setFormat(format);
	
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(m_display, 0,0, 1, 5);
    gridLayout->addLayout(hboxLayout, 1, 0);

    QLabel* label = new QLabel("View", this);
    m_viewPerspective = new QComboBox(this);
	Q_CHECK_PTR(label);
	Q_CHECK_PTR(m_viewPerspective);
    label->setBuddy(m_viewPerspective);

    hboxLayout->addWidget(label);
    hboxLayout->addWidget(m_viewPerspective);

    label = new QLabel("Drawing Mode", this);
    m_drawingMode = new QComboBox(this);
	Q_CHECK_PTR(label);
	Q_CHECK_PTR(m_drawingMode);
    label->setBuddy(m_drawingMode);

    hboxLayout->setContentsMargins(11, 0, 0, 0);
    hboxLayout->addWidget(label);
    hboxLayout->addWidget(m_drawingMode);
	QSpacerItem* spacer = new QSpacerItem(100, 0);
	Q_CHECK_PTR(spacer);
    hboxLayout->addItem(spacer);
    gridLayout->setColumnStretch(0,0);
    gridLayout->setColumnStretch(1,0);
    gridLayout->setColumnStretch(2,0);
    gridLayout->setColumnStretch(3,0);
    gridLayout->setColumnStretch(4,4);

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
	m_drawingMode->addItem("Textured", Texture);
    m_drawingMode->setCurrentIndex(2);
    m_display->setDrawingMode( Flat );

    connect(m_viewPerspective, SIGNAL(currentIndexChanged(int)), this, SLOT(viewPerspectiveChanged(int)));
    connect(m_drawingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(drawingModeChanged(int)));

}

void DocumentView::setDocument(IDocument* doc) {
	Q_ASSERT(doc);
	
	if (m_document)
		m_document->disconnect(this);
    m_document = doc;
    connect(m_document, SIGNAL(changed(IDocument::ChangeType, IObject3D*)), this, SLOT(updateView()));
};

void DocumentView::setGridVisible( bool value)
{
	Q_ASSERT(m_display);
    m_display->setGridVisible( value );
}

bool DocumentView::isGridVisible()
{
	Q_ASSERT(m_display);
    return m_display->isGridVisible();
}

void DocumentView::updateView()
{
	Q_ASSERT(m_display);
    m_display->updateGL();
}

void DocumentView::viewPerspectiveChanged(int index)
{
	Q_ASSERT(m_display);
	Q_ASSERT(m_viewPerspective);
	
    GlDisplay::PerspectiveType type;

    type = (GlDisplay::PerspectiveType)m_viewPerspective->itemData(index).toInt();
    m_display->setPerspectiveView( type );
    updateView();
}

void DocumentView::drawingModeChanged(int index)
{
	Q_ASSERT(m_display);
	Q_ASSERT(m_viewPerspective);
	
    DrawingMode type;

    type = (DrawingMode)m_viewPerspective->itemData(index).toInt();
    m_display->setDrawingMode( type );
    updateView();
}

void DocumentView::setNormalsVisible(bool value)
{
	Q_ASSERT(m_display);
    m_display->setNormalsVisible(value);
}

