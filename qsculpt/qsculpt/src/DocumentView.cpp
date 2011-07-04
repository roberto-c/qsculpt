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
#include "glitem.h"
#include "GlView.h"
#include "CustomGLContext.h"

struct DocumentView::Impl {
    CustomGLContext ctx;
};

DocumentView::DocumentView(QWidget *_parent)
: QWidget(_parent),
_document(NULL),
_viewPerspective(NULL),
_drawingMode(NULL),
_drawVertices(false),
_d(new Impl)
{
    createWidgets();
}

DocumentView::~DocumentView()
{
}

void DocumentView::createWidgets()
{
    QGridLayout* gridLayout = new QGridLayout(this);
    QHBoxLayout* hboxLayout = new QHBoxLayout();
    
    _d->ctx.setUseSoftwareRenderer(true);
    _d->ctx.create();
    _display = new GlCanvas(&_d->ctx, this);

    Q_CHECK_PTR(gridLayout);
    Q_CHECK_PTR(_display);
    Q_CHECK_PTR(hboxLayout);

    // Set up gl display format
    QGLFormat format = _display->format();
    format.setSwapInterval(1);
    format.setDepthBufferSize(32);
    _display->setFormat(format);

    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(_display, 0, 0, 1, 5);
    gridLayout->addLayout(hboxLayout, 1, 0);
    
    QLabel* label = new QLabel("View", this);
    _viewPerspective = new QComboBox(this);
    Q_CHECK_PTR(label);
    Q_CHECK_PTR(_viewPerspective);
    label->setBuddy(_viewPerspective);

    hboxLayout->addWidget(label);
    hboxLayout->addWidget(_viewPerspective);

    label = new QLabel("Drawing Mode", this);
    _drawingMode = new QComboBox(this);
    Q_CHECK_PTR(label);
    Q_CHECK_PTR(_drawingMode);
    label->setBuddy(_drawingMode);

    hboxLayout->setContentsMargins(11, 0, 0, 0);
    hboxLayout->addWidget(label);
    hboxLayout->addWidget(_drawingMode);
    QSpacerItem* spacer = new QSpacerItem(100, 0);
    Q_CHECK_PTR(spacer);
    hboxLayout->addItem(spacer);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    gridLayout->setColumnStretch(4, 4);

    _viewPerspective->addItem("Front", GlCanvas::Front);
    _viewPerspective->addItem("Back", GlCanvas::Back);
    _viewPerspective->addItem("Top", GlCanvas::Top);
    _viewPerspective->addItem("Bottom", GlCanvas::Bottom);
    _viewPerspective->addItem("Left", GlCanvas::Left);
    _viewPerspective->addItem("Right", GlCanvas::Right);
    _viewPerspective->addItem("Perspective", GlCanvas::Perspective);
    _viewPerspective->setCurrentIndex(6);
    _display->setPerspectiveView(GlCanvas::Perspective);

    _drawingMode->addItem("Points", Points);
    _drawingMode->addItem("Wireframe", Wireframe);
    _drawingMode->addItem("Flat", Flat);
    _drawingMode->addItem("Smooth", Smooth);
    _drawingMode->addItem("Textured", Texture);
    _drawingMode->setCurrentIndex(2);
    _display->setDrawingMode(Points);

    connect(_viewPerspective, SIGNAL(currentIndexChanged(int)), this, SLOT(viewPerspectiveChanged(int)));
    connect(_drawingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(drawingModeChanged(int)));
}

void DocumentView::setDocument(IDocument* doc)
{
    Q_ASSERT(doc);

    if (_document)
        _document->disconnect(this);
    _document = doc;
    connect(_document, SIGNAL(changed(IDocument::ChangeType, ISurface*)), this, SLOT(updateView()));
};

void DocumentView::setGridVisible(bool value)
{
    Q_ASSERT(_display);
    _display->setGridVisible(value);
}

bool DocumentView::isGridVisible()
{
    Q_ASSERT(_display);
    return _display->isGridVisible();
}

void DocumentView::updateView()
{
    Q_ASSERT(_display);
    _display->updateGL();
}

void DocumentView::viewPerspectiveChanged(int index)
{
    Q_ASSERT(_display);
    Q_ASSERT(_viewPerspective);

    GlCanvas::PerspectiveType type;

    type = (GlCanvas::PerspectiveType)_viewPerspective->itemData(index).toInt();
    _display->setPerspectiveView(type);
    updateView();
}

void DocumentView::drawingModeChanged(int index)
{
    Q_ASSERT(_display);
    Q_ASSERT(_viewPerspective);

    DrawingMode type;

    type = (DrawingMode) _viewPerspective->itemData(index).toInt();
    _display->setDrawingMode(type);
    updateView();
}

void DocumentView::setNormalsVisible(bool value)
{
    Q_ASSERT(_display);
    _display->setNormalsVisible(value);
}

void DocumentView::setDrawVertices(bool drawVertices)
{
    _drawVertices = drawVertices;
}

bool DocumentView::getDrawVertices()
{
    return _drawVertices;
}

IRenderer* DocumentView::renderer() const
{
    return _display->renderer();
}
