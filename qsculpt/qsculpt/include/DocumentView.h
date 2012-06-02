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
#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QtGui/QWidget>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include "Point3D.h"
#include "GlView.h"
#include "IDocument.h"

class ISurface;
class IDocument;
class QComboBox;

/**
 * The document is the raw structure of the data, in other words, the document
 * for it self do not have a graphical representation. So, this class provides
 * a graphical representation for that data.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class DocumentView : public QWidget
{
    Q_OBJECT

public:
    /**
     * Widget contructor.
     *
     * @param parent widget's parent.
     */
    DocumentView(QWidget *parent = 0);

    /**
     * Widget destructor.
     */
    virtual ~DocumentView();

    /**
     * Create and initialize all UI objects.
     */
    void createWidgets();
    /**
     * Set document that contains all the objects to draw.
     *
     * @param doc document from which to get objects to draw.
     */
    void setDocument(IDocument::shared_ptr doc);

    /**
     * Get the view's document source.
     *
     * @return source document for this view.
     */
    IDocument::shared_ptr getDocument() {
        return _document;
    };

    /**
     * Return the state of the visibility of the grid.
     *
     * @return true if grid is visible. False otherwise.
     */
    bool isGridVisible();

    bool areNormalsVisible();

    /**
     * Retuns a collection of objects under the position (x, y).
     *
     * @param x x component of the point to test.
     * @param y y component of the point to test.
     *
     * @return an object container with the results of the test.
     */
	ObjectContainer getSelectedObjects(int x, int y) {
        return _display->getSelectedObjects(x, y);
    };
	PointIndexList getSelectedVertices(GLint x, GLint y,
									   GLint width, GLint height) {
		return _display->getSelectedVertices(x, y, width, height);
	}

    /**
     * getViewType
     */
    GlCanvas::PerspectiveType getPerspectiveViewType() {
        return _display->getPerspectiveView();
    };

    /**
     *
     */
    Camera* getViewCamera() {
        return _display->getViewCamera();
    };

    void set3DCursorShape(GlCanvas::CursorShapeType shape) {
        _display->set3DCursorShape(shape);
    };

    GlCanvas::CursorShapeType getCursorShape() {
        return _display->getCursorShape();
    };

    void setCursorPosition(Point3 p) {
        _display->setCursorPosition(p);
    };

    Point3 getCursorPosition() {
        return _display->getCursorPosition();
    };

    void setCursorOrientation(Point3 n) {
        _display->setCursorOrientation(n);
    };

    Point3 getCursorOrientation() {
        return _display->getCursorOrientation();
    };

    void setCursorImage(const QImage& image) {
    	_display->setCursorImage(image);
    }

    QImage getCursorImage() {
    	return _display->getCursorImage();
    }

    bool getDrawVertices();
    
    GlCanvas* getCanvas() { return _display; } ;
    
    IRenderer* renderer() const;
    
public slots:
    /**
     * Turn the visibility of the grid on/off.
     *
     * @param value true to turn on the grid. False to turn off.
     */
    void setGridVisible(bool value);

    /**
     * Change the view perspective of the display.
     */
    void viewPerspectiveChanged(int index);

    /**
     *
     */
    void drawingModeChanged(int index);

    /**
     *
     */
    void setNormalsVisible(bool value);

    /**
     * This method causes a repaint of the view. This should be called when
     * data has changed.
     */
    void updateView();

    void grabMouse(bool val) {
    	if (_display)
    	{
    		if (val)
    			_display->grabMouse();
    		else
    			_display->releaseMouse();
    	}
    }

    void setDrawVertices(bool drawVertices);
    
protected:
    IDocument::shared_ptr       _document;
    GlCanvas        *_display;
    QComboBox       *_viewPerspective;
    QComboBox       *_drawingMode;
    bool            _drawVertices;
    
    class Impl;
    QScopedPointer<Impl> _d;
};

#endif

