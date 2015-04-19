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
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>

#include <PlastilinaCore/subdivision/Sphere.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/IDocument.h>
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "ICommand.h"
#include "glitem.h"
#include "GlView.h"
#include "CustomGLContext.h"

struct DocumentView::Impl {
    IDocument::shared_ptr    _document;
    GlCanvas        		*_display;
    QComboBox       		*_viewPerspective;
    QComboBox       		*_drawingMode;
    bool            		 _drawVertices;
    CustomGLContext 		 ctx;
};

DocumentView::DocumentView(QWidget *_parent)
: QWidget(_parent),
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
    
    // Set up gl display format
    QGLFormat format = _d->ctx.format();
    format.setProfile(QGLFormat::CoreProfile);
    format.setVersion(3, 2);
    format.setSwapInterval(1);
    format.setDepthBufferSize(32);
    _d->ctx.setFormat(format);
    _d->ctx.setUseSoftwareRenderer(false);
    _d->ctx.create();
    _d->_display = new GlCanvas(&_d->ctx, this);
    //_display = new GlCanvas(this);

    Q_CHECK_PTR(gridLayout);
    Q_CHECK_PTR(_d->_display);
    Q_CHECK_PTR(hboxLayout);

    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(_d->_display, 0, 0, 1, 5);
    gridLayout->addLayout(hboxLayout, 1, 0);
    
    QLabel* label = new QLabel("View", this);
    _d->_viewPerspective = new QComboBox(this);
    Q_CHECK_PTR(label);
    Q_CHECK_PTR(_d->_viewPerspective);
    label->setBuddy(_d->_viewPerspective);

    hboxLayout->addWidget(label);
    hboxLayout->addWidget(_d->_viewPerspective);

    label = new QLabel("Drawing Mode", this);
    _d->_drawingMode = new QComboBox(this);
    Q_CHECK_PTR(label);
    Q_CHECK_PTR(_d->_drawingMode);
    label->setBuddy(_d->_drawingMode);

    hboxLayout->setContentsMargins(11, 0, 0, 0);
    hboxLayout->addWidget(label);
    hboxLayout->addWidget(_d->_drawingMode);
    QSpacerItem* spacer = new QSpacerItem(100, 0);
    Q_CHECK_PTR(spacer);
    hboxLayout->addItem(spacer);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    gridLayout->setColumnStretch(4, 4);

    _d->_viewPerspective->addItem("Front", GlCanvas::Front);
    _d->_viewPerspective->addItem("Back", GlCanvas::Back);
    _d->_viewPerspective->addItem("Top", GlCanvas::Top);
    _d->_viewPerspective->addItem("Bottom", GlCanvas::Bottom);
    _d->_viewPerspective->addItem("Left", GlCanvas::Left);
    _d->_viewPerspective->addItem("Right", GlCanvas::Right);
    _d->_viewPerspective->addItem("Perspective", GlCanvas::Perspective);
    _d->_viewPerspective->addItem("Camera", GlCanvas::CameraView);
    _d->_viewPerspective->setCurrentIndex(7);
    _d->_display->setPerspectiveView(GlCanvas::CameraView);

    _d->_drawingMode->addItem("Points", Points);
    _d->_drawingMode->addItem("Wireframe", Wireframe);
    _d->_drawingMode->addItem("Flat", Flat);
    _d->_drawingMode->addItem("Smooth", Smooth);
    _d->_drawingMode->addItem("Textured", Texture);
    _d->_drawingMode->setCurrentIndex(2);
    _d->_display->setDrawingMode(Flat);

    connect(_d->_viewPerspective, SIGNAL(currentIndexChanged(int)), this, SLOT(viewPerspectiveChanged(int)));
    connect(_d->_drawingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(drawingModeChanged(int)));
}

void DocumentView::setDocument(IDocument::shared_ptr doc)
{
    Q_ASSERT(doc);

    _d->_document = doc;
    auto scene = _d->_document->scene().lock();
    if (scene) {
        std::shared_ptr<Camera> cam;
        cam = scene->getCamera() ? scene->getCamera()->camera() : nullptr;
        if (cam) {
        	_d->_display->setViewCamera(GlCanvas::CameraView, cam);
        }
    }
};

void DocumentView::setGridVisible(bool value)
{
    Q_ASSERT(_d->_display);
    _d->_display->setGridVisible(value);
}

bool DocumentView::isGridVisible()
{
    Q_ASSERT(_d->_display);
    return _d->_display->isGridVisible();
}

void DocumentView::updateView()
{
    Q_ASSERT(_d->_display);
    _d->_display->updateGL();
}

void DocumentView::viewPerspectiveChanged(int index)
{
    Q_ASSERT(_d->_display);
    Q_ASSERT(_d->_viewPerspective);

    GlCanvas::PerspectiveType type;

    type = (GlCanvas::PerspectiveType)(_d->_viewPerspective->itemData(index).toInt());
    _d->_display->setPerspectiveView(type);
    updateView();
}

void DocumentView::drawingModeChanged(int index)
{
    Q_ASSERT(_d->_display);
    Q_ASSERT(_d->_viewPerspective);

    DrawingMode type;

    type = (DrawingMode) (_d->_viewPerspective->itemData(index).toInt());
    _d->_display->setDrawingMode(type);
    updateView();
}

void DocumentView::setNormalsVisible(bool value)
{
    Q_ASSERT(_d->_display);
    _d->_display->setNormalsVisible(value);
}

void DocumentView::setDrawVertices(bool drawVertices)
{
    _d->_drawVertices = drawVertices;
}

bool DocumentView::getDrawVertices()
{
    return _d->_drawVertices;
}

GlCanvas* DocumentView::getCanvas()
{
    return _d->_display;
}


IRenderer* DocumentView::renderer() const
{
    return _d->_display->renderer();
}

IDocument::shared_ptr DocumentView::getDocument() {
    return _d->_document;
};

ObjectContainer DocumentView::getSelectedObjects(int x, int y) {
    return _d->_display->getSelectedObjects(x, y);
};
PointIndexList DocumentView::getSelectedVertices(GLint x, GLint y,
                                   GLint width, GLint height) {
    return _d->_display->getSelectedVertices(x, y, width, height);
}

GlCanvas::PerspectiveType DocumentView::getPerspectiveViewType() {
    return _d->_display->getPerspectiveView();
};

std::shared_ptr<Camera> DocumentView::getViewCamera() {
    return _d->_display->getViewCamera();
};

void DocumentView::set3DCursorShape(GlCanvas::CursorShapeType shape) {
    _d->_display->set3DCursorShape(shape);
};

GlCanvas::CursorShapeType DocumentView::getCursorShape() {
    return _d->_display->getCursorShape();
};

void DocumentView::setCursorPosition(Point3 p) {
    _d->_display->setCursorPosition(p);
};

Point3 DocumentView::getCursorPosition() {
    return _d->_display->getCursorPosition();
};

void DocumentView::setCursorOrientation(Point3 n) {
    _d->_display->setCursorOrientation(n);
};

Point3 DocumentView::getCursorOrientation() {
    return _d->_display->getCursorOrientation();
};

void DocumentView::setCursorImage(const QImage& image) {
    _d->_display->setCursorImage(image);
}

QImage DocumentView::getCursorImage() {
    return _d->_display->getCursorImage();
}

void DocumentView::grabMouse(bool val) {
    if (_d->_display)
    {
        if (val)
            _d->_display->grabMouse();
        else
            _d->_display->releaseMouse();
    }
}

