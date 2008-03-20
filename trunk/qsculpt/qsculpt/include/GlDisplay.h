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
#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <QGLWidget>
#include <QPoint>
#include <QVector>
#include "SpEnums.h"
#include "Point3D.h"
#include "BufferObject.h"

class DocumentView;
class QMouseEvent;
class QWheelEvent;
class Camera;
class IRenderer;

/**
 * Structure used to hold a hit return.
 */
struct HitRecord
{
    int numNamesInStack;
    int minDepth;
    int maxDepth;
    int stackContents;
};

/**
 * Widget to display OpenGL graphics. Setup the device context, handle resize
 * events, mouse events, etc.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class GlDisplay : public QGLWidget
{
    Q_OBJECT
public:
    typedef QMap<int, Camera*> CameraContainer;

    enum PerspectiveType {
        Front = 0,
        Back,
        Top,
        Bottom,
        Left,
        Right,
        Perspective
    };

    enum CursorShapeType {
        None,
        Cross,
        Circle,
        Image
    };

    /**
     * Widget constructor. Initializes default parameters. This widget only can
     * have a DocumentView class-derived as parent.
     *
     * @param parent the parent view of the widget.
     */
    GlDisplay(DocumentView* parent = 0);

    /**
     * Default destructor
     */
    ~GlDisplay();

    /**
     * Return the state of the visibility of the grid.
     *
     * @return true if grid is visible. False otherwise.
     */
    bool isGridVisible();

    /**
     *
     */
    bool areNormalsVisible();

    /**
     * Get a list of pick records as returned by glRender. This is used to
     * support picking or selection on objects. Each record contains
     * information about the object under the (x, y) coordinates. The passed
     * coordinates are in window space coordinates.
     *
     * @param x coord's x component
     * @param y coord's y component
     *
     * @return HitRecord vector
     */
    QVector<HitRecord> getPickRecords(int _x, int _y);

    /**
     */
    Camera* getViewCamera();

    /**
     */
    PerspectiveType getPerspectiveView() {
        return m_viewType;
    };

    void set3DCursorShape(CursorShapeType shape);

    CursorShapeType getCursorShape() {
        return m_cursorShape;
    };

    void setCursorPosition(Point3D p) {
        m_cursorPosition = p;
    };

    Point3D getCursorPosition() {
        return m_cursorPosition;
    };

    void setCursorOrientation(Point3D n) {
        m_cursorOrientation = n;
    };

    Point3D getCursorOrientation() {
        return m_cursorOrientation;
    };

    /**
     * Sets the image to show as the cursor. The image is copied to a new
     * image.
     */
    void setCursorImage(const QImage& image);

    /**
     * Returns a copy of the image used as cursor.
     */
    QImage getCursorImage();

public slots:
    /**
     * Set the drawing mode of the display.
     *
     * @param mode New drawing mode to display.
     */
    void setDrawingMode(DrawingMode mode);

    /**
     * Change the view perspective to one of the predefined type.
     *
     * @param type new view to use
     */
    void setPerspectiveView(PerspectiveType type) {
        m_viewType = type;
    };

    /**
     * Turn the visibility of the grid on/off.
     *
     * @param value true to turn on the grid. False to turn off.
     */
    void setGridVisible(bool value);

    /**
     *
     */
    void setNormalsVisible(bool value);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mouseMoveEvent ( QMouseEvent * e );
    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent ( QMouseEvent * e );
    void wheelEvent ( QWheelEvent * e );

    bool m_mousePressed;
    QPoint m_mousePosition;

private:
    /**
     * Draws the grid of the widget.
     */
    void drawGrid();

    void drawObjects();

    void drawCursor();

    void drawOrientationAxis();

    void mapScreenCoordsToWorldCoords(int x, int y, int z, double *wx, double *wy, double *wz);
    void mapWorldCoordsToScreenCoords(double wx, double wy, double wz, int *x, int *y, int *z);

    bool            m_isGridVisible;        /**< Grid visibility flag */
    bool            m_areNormalsVisible;    /**< Normals visibility flag */
    GLuint*         m_selectBuffer;         /**< Selection buffer */
    double          m_aspectRatio;
    PerspectiveType m_viewType;             /**< Kind of view to display */
    DrawingMode     m_drawingMode;          /**< Object drawing mode */
    IRenderer*		m_renderer;				/**< Rendering engine for the objects */
	IRenderer*		m_selectionRenderer;	/**< Renderer used for selection. */
    CameraContainer m_cameraList;           /**< Cameras for the differents view types */

    CursorShapeType     m_cursorShape;
    Point3D             m_cursorPosition;
    Point3D             m_cursorOrientation;
    GLint               m_viewport[4];
    GLfloat             m_zoomFactor;
    GLuint				m_textureId;
    QImage				m_cursorImage;
};

#endif

