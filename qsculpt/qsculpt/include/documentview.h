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

#include <QWidget>
#include <QList>
#include <QPoint>
#include "point3d.h"
#include "gldisplay.h"

class IObject3D;
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
    void setDocument(IDocument* doc) { 
        m_document = doc; 
    };
    
    /**
     * Get the view's document source. 
     *
     * @return source document for this view.
     */
    IDocument* getDocument() {
        return m_document; 
    };
    
    /**
     * Return the state of the visibility of the grid.
     *
     * @return true if grid is visible. False otherwise.
     */
    bool isGridVisible();
    
    bool areNormalsVisible();
    
    /**
     * update view display.
     */
    void updateView();
    
    /**
     * Retuns a collection of hit record. Each hit record contains information
     * about the objects under an especific point on the screen.
     * 
     * @param x x component of the point to test.
     * @param y y component of the point to test.
     * 
     * @return a HitRecord vector with the results of the test.
     */
    QVector<HitRecord> getPickRecords(int x, int y) { 
        return m_display->getPickRecords(x, y);
    };
    
    /**
     * getViewType
     */
    GlDisplay::PerspectiveType getPerspectiveViewType() { 
        return m_display->getPerspectiveView();
    };
    
    /**
     * 
     */
    Camera* getViewCamera() { 
        return m_display->getViewCamera();
    };
    
    void set3DCursorShape(GlDisplay::CursorShapeType shape) {
        m_display->set3DCursorShape(shape);
    };

    GlDisplay::CursorShapeType getCursorShape() {
        return m_display->getCursorShape();
    };
    
    void setCursorPosition(Point3D p) { 
        m_display->setCursorPosition(p); 
    };

    Point3D getCursorPosition() {
        return m_display->getCursorPosition();
    };

    void setCursorOrientation(Point3D n) {
        m_display->setCursorOrientation(n);
    };

    Point3D getCursorOrientation() {
        return m_display->getCursorOrientation();
    };

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
    
protected:
    IDocument* m_document;
    GlDisplay* m_display;
    QComboBox* m_viewPerspective;
    QComboBox* m_drawingMode;
};

#endif

