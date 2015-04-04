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
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/IDocument.h>
#include "GlView.h"

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
    IDocument::shared_ptr getDocument() ;

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
	ObjectContainer getSelectedObjects(int x, int y);
    
	PointIndexList getSelectedVertices(GLint x, GLint y,
                                       GLint width, GLint height);

    /**
     * getViewType
     */
    GlCanvas::PerspectiveType getPerspectiveViewType();

    /**
     *
     */
    std::shared_ptr<Camera> getViewCamera();

    void set3DCursorShape(GlCanvas::CursorShapeType shape);

    GlCanvas::CursorShapeType getCursorShape();

    void setCursorPosition(Point3 p);

    Point3 getCursorPosition();

    void setCursorOrientation(Point3 n);

    Point3 getCursorOrientation();

    void setCursorImage(const QImage& image);

    QImage getCursorImage();

    bool getDrawVertices();
    
    GlCanvas* getCanvas();
    
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

    void grabMouse(bool val);

    void setDrawVertices(bool drawVertices);
    
protected:    
    struct Impl;
    QScopedPointer<Impl> _d;
};

#endif

