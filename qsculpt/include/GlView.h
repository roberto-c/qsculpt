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

#include <PlastilinaCore/opengl/OpenGL.h>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QPoint>
#include <QtCore/QVector>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <PlastilinaCore/SpEnums.h>
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/BufferObject.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/Scene.h>

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
class GlCanvas : public QGLWidget
{
    Q_OBJECT
public:
    enum PerspectiveType {
        Front = 0,
        Back,
        Top,
        Bottom,
        Left,
        Right,
        Perspective,
        CameraView
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
    GlCanvas(DocumentView* parent = 0);
    
    /**
     * Widget constructor. Initializes default parameters. This widget only can
     * have a DocumentView class-derived as parent.
     *
     * @param parent the parent view of the widget.
     */
    GlCanvas(QGLContext * ctx, DocumentView* parent = 0);

    /**
     * Default destructor
     */
    ~GlCanvas();

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
     * Get a list of objects drawn under the position (x, y).
     *
     * @param x coord's x component
     * @param y coord's y component
     *
     * @return HitRecord vector
     */
    ObjectContainer getSelectedObjects(GLint x, GLint y);

    /**
     * This method returns the vertices contained inside the specified 2D box
     * in screen coordinates.
     */
    PointIndexList getSelectedVertices(GLint x, GLint y,
                                       GLint width, GLint height);
    /**
     * Return the current active camera in the canvas.
     */
    std::shared_ptr<Camera> getViewCamera();

    /**
     * Return the current active camera in the canvas.
     */
    const std::shared_ptr<Camera> getViewCamera() const;
    
    /**
     * Function to overtide the camera view ot use for each default view.
     *
     * Default view are ok, except fot eh camera view that should be set each 
     * time the default camera of the document changes.
     */
    void setViewCamera(PerspectiveType viewType,
                       std::shared_ptr<Camera> & camera);
    
    /**
     * Map a point in screen coordinate system to world coordinate system
     * using current active camera settings for doing the mapping.
     */
    void screenToWorld(double x, double y, double z,
                                      double *wx, double *wy, double *wz) const;
    /**
     * Map a point in screen coordinate system to world coordinate system
     * using current active camera settings for doing the mapping.
     */
    void screenToWorld(const Point3&, Point3&) const;
    
    /**
     * Map a point in screen coordinate system to world coordinate system
     * using current active camera settings for doing the mapping.
     */
    Point3 screenToWorld(const Point3&) const;
    
    /**
     * Map a point in world coordinate system to screen coordinate system
     * using current active camera settings for doing the mapping.
     */
    void worldToScreen(double wx, double wy, double wz,
                                      double *x, double *y, double *z) const;
    /**
     * Map a point in worls coordinate system to screen coordinate system
     * using current active camera settings for doing the mapping.
     */
    void worldToScreen(const Point3&, Point3&) const;
    
    /**
     * Map a point in worls coordinate system to screen coordinate system
     * using current active camera settings for doing the mapping.
     */
    Point3 worldToScreen(const Point3&) const;
    
    /**
     *
     */
    PerspectiveType getPerspectiveView() ;

    void set3DCursorShape(CursorShapeType shape);

    CursorShapeType getCursorShape();

    void setCursorPosition(Point3 p);

    Point3 getCursorPosition() ;

    void setCursorOrientation(Point3 n);

    Point3 getCursorOrientation() ;

    /**
     * Sets the image to show as the cursor. The image is copied to a new
     * image.
     */
    void setCursorImage(const QImage& image);

    /**
     * Returns a copy of the image used as cursor.
     */
    QImage getCursorImage();

    /**
     *
     */
    void enable(GLenum flag) { glEnable(flag); }
    
    /**
     *
     */
    void disable(GLenum flag) { glDisable(flag); }
    
    /**
     * This function sets the QPen to use to draw different primitves.
     *
     * Following drawing command will use the color, width and more properties
     * of provided QPen to draw.
     */
    void setPen(const QPen& pen);
    
    /**
     * This function return the QPen used to draw.
     */
    const QPen& pen() const;
    
    /**
     * This function sets the QBrush to use to draw different primitves.
     *
     * Following drawing command will use the color, width and more properties
     * of provided QBrush to draw.
     */
    void setBrush(const QBrush& pen);
    
    /**
     * This function return the QBrush used to draw.
     */
    const QBrush& brush() const;
    
    /**
     * Returns the color under pixel at coordinate p. This function 
     * ignores Z component.
     */
    Color color(const Point3 & p);
    
    /**
     * Returns the color under pixel at screen coordinate (x,y). 
     */
    Color color(int x, int y);
    
    /**
     * Returns the color under pixel at coordinate p. This function 
     * ignores Z component.
     */
    uint32_t colorUint(const Point3 & p);
    
    /**
     * Returns the color under pixel at screen coordinate (x,y). 
     */
    uint32_t colorUint(int x, int y);
    
    /**
     * Returns the depth component at p in screen coordinates. This function 
     * ignores Z component.
     */
    float depth(const Point3 & p);
    
    /**
     * Returns the depth component at screen coordinates (x,y).
     */
    float depth(int x, int y);
    
    /**
     *
     */
    void drawLine(const Point3& p1, const Point3& p2);
    
    /**
     * Draw a line in screen coordinates
     */
    void drawLine2D(const Point3& p1, const Point3& p2);
    
    /**
     * Draw a rectangle into the canvas. The z coordinatinate is assumed to be
     * equal between both points.
     */
    void drawRect(const Point3& p1, const Point3& p2, int mode = 0);
    
    /**
     * Draw an ellipse with an optional hole in it. The ellipse is defined
     * by the major axis and the minor axis.
     *
     * @param axis1 Length of the long axis in pixels.
     * @param axis2 Length of the minor axis in pixels.
     * @param innerAxis1 Length of the long axis of the inner ellipse. Pass 0
     *                   if not inner ellipse is desired.
     * @param innerAxis2 Length of the minor axis of the inner ellipse. Pass 0
     *                   if not inner ellipse is desired.
     */
    void drawEllipse(const Point3& center, 
                     float axis1,
                     float axis2,
                     float innerAxis1 = 0.0f,
                     float innerAxis2 = 0.0f);
    
    /**
     * Draw an arc, a part of an ellipse with an optional hole in it. The ellipse
     * is defined by the major axis and the minor axis. The arc is defined by the
     * start and end angles.
     *
     * @param center center of the arc
     * @param starAngle start angle of the arc in degrees.
     * @param endAngle end angle of the arc in degrees.
     * @param axis1 Length of the long axis in pixels.
     * @param axis2 Length of the minor axis in pixels.
     * @param innerAxis1 Length of the long axis of the inner ellipse. Pass 0
     *                   if not inner ellipse is desired.
     * @param innerAxis2 Length of the minor axis of the inner ellipse. Pass 0
     *                   if not inner ellipse is desired.
     */
    void drawArc(const Point3& center, 
                 float startAngle,
                 float endAngle,
                 float axis1,
                 float axis2,
                 float innerAxis1 = 0.0f,
                 float innerAxis2 = 0.0f);
    
    /**
     * This method gets the current renderer used to render surfaces.
     */
    IRenderer* renderer() const ;
    
    /**
     * Used to draw a scene hierarchy using the current renderer
     */
    void drawScene(Scene::shared_ptr scene);
    
    
    /**
     * Used to submit data to hw. Similar to glBegin()
     *
     * Used to emulate glBegin()/ glEnd() from OpenGL. It will use temporary
     * buffer to send data to GPU.
     */
    void begin(GLenum mode);
    
    /**
     *
     */
    void end();
    
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
    void setPerspectiveView(PerspectiveType type);
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

    bool _mousePressed;
    QPoint _mousePosition;

private:
    /**
     * Draws the grid of the widget.
     */
    void drawGrid();
    
    /**
     * Draw a bounding box a round the surface.
     */
    void drawBoundingBox(const ISurface* mesh);

    void drawCursor();

    void drawOrientationAxis();
    
    void drawRectWinCoord(const Point3&, const Point3& );
        
    void drawEllipseWinCoord(const Point3& center,
                             float startAngle,
                             float endAngle,
                             float axis1,
                             float axis2,
                             float innerAxis1,
                             float innerAxis2);
    
    void drawSceneNode(Scene::shared_ptr & scene,
                       SceneNode::shared_ptr & node);
	
    struct Impl;
    QScopedPointer<Impl> _d;
    
};

#endif

