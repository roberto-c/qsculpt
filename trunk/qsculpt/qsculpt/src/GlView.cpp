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
#include "GlView.h"
#include <QtOpenGL/QtOpenGL>
#include <iostream>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtCore/QtAlgorithms>
#include <QtCore/QLineF>
#include <QtCore/QRectF>
#include <QtGui/QPolygonF>
#include <QtGui/QMatrix>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include "Sphere.h"
#include "Box.h"
#include "IDocument.h"
#include "QSculptWindow.h"
#include "QSculptApp.h"
#include "ICommand.h"
#include "DocumentView.h"
#include "Camera.h"
#include "RendererFactory.h"
#include "Picking.h"
#include "PickingFacesRenderer.h"
#include "Scene.h"
#include "SceneNode.h"
#include "GlslProgram.h"
#include "GlslShader.h"

PickingObjectRenderer g_picking;
PickingFacesRenderer g_pickingVertices;

#define SELECT_BUFFER_SIZE 512
#define DEFAULT_HEIGHT 5.0f

GlslProgram     *g_shaderProgram = NULL;
VertexShader    *g_shaderVertex = NULL;
FragmentShader  *g_shaderFragment = NULL;


struct Rect {
    GLfloat x1, y1, x2, y2;

    Rect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {
    }
};

#define GLCANVAS_BEGIN  (0x0001)

struct GlCanvas::Impl {
    bool            isGridVisible;        /**< Grid visibility flag */
    bool            areNormalsVisible;    /**< Normals visibility flag */
    GLuint*         selectBuffer;         /**< Selection buffer */
    double          aspectRatio;
    PerspectiveType viewType;             /**< Kind of view to display */
    DrawingMode     drawingMode;          /**< Object drawing mode */
    IRenderer*		renderer;				/**< Rendering engine for the objects */
    IRenderer*		selectionRenderer;	/**< Renderer used for selection. */
    IRenderer*      editVertexRenderer;    /**< Renderer used for vertex edition */
    CameraContainer cameraList;           /**< Cameras for the differents view types */

    CursorShapeType     cursorShape;
    Point3              cursorPosition;
    Point3              cursorOrientation;
    GLint               viewport[4];
    GLfloat             zoomFactor;
    GLuint              textureId;
    QImage              cursorImage;
    QPen                pen;
    QBrush              brush;
    uint                flags;
    
    Impl() : isGridVisible(false),
    areNormalsVisible(false),
    selectBuffer(NULL),
    aspectRatio(1.0),
    viewType(Front),
    drawingMode(Points),
    renderer(NULL),
    selectionRenderer(NULL),
    editVertexRenderer(NULL),
    cursorShape(None),
    zoomFactor(1.0),
    textureId(0),
    pen(QPen(QColor(Qt::white))),
    brush(QBrush(Qt::transparent)),
    flags(0)
    {
        // Type of renderer used for displaying objects on the screen
        renderer = RendererFactory::getRenderer(drawingMode);
        
        // Type of renderer used for user object picking
        selectionRenderer = RendererFactory::getRenderer(Points);
        
        // Type of renderer used for vertex edition mode
        editVertexRenderer = RendererFactory::getRenderer(Points);
        
        selectBuffer = new GLuint[SELECT_BUFFER_SIZE];
        
        Camera* camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector(Point3( 0, 1, 0) );
        camera->setPosition( Point3( 0, 0, 1));
        cameraList[Front] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 1, 0) );
        camera->setPosition( Point3( 0, 0, -1));
        cameraList[Back] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 0, -1) );
        camera->setPosition( Point3( 0, 1, 0) );
        cameraList[Top] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 0, 1) );
        camera->setPosition( Point3( 0, -1, 0) );
        cameraList[Bottom] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 1, 0) );
        camera->setPosition( Point3(-1, 0, 0) );
        cameraList[Left] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 1, 0) );
        camera->setPosition( Point3( 1, 0, 0) );
        cameraList[Right] = camera;
        
        camera = new Camera();
        camera->setTargetPoint( Point3( 0, 0, 0) );
        camera->setOrientationVector( Point3( 0, 0, 1) );
        camera->setPosition( Point3( 0.75, 0.75, 0.75) );
        cameraList[Perspective] = camera;
    }
    
    ~Impl() {
        if (selectBuffer)
            delete [] selectBuffer;
        
        QMutableMapIterator<int, Camera*> it(cameraList);
        while(it.hasNext())
        {
            it.next();
            delete it.value();
            it.remove();
        }
        
        delete renderer;
        renderer = NULL;
        
        delete selectionRenderer;
        selectionRenderer = NULL;
    }
};

GlCanvas::GlCanvas(DocumentView* _parent)
: QGLWidget(_parent), _d(new Impl)
{
    
}


GlCanvas::GlCanvas(QGLContext * ctx, DocumentView* _parent)
    : QGLWidget(ctx, _parent), _d(new Impl)
    
{
}


GlCanvas::~GlCanvas()
{
}

GlCanvas::PerspectiveType GlCanvas::getPerspectiveView() {
    return _d->viewType;
}

GlCanvas::CursorShapeType GlCanvas::getCursorShape() {
    return _d->cursorShape;
}

void GlCanvas::setCursorPosition(Point3 p) {
    _d->cursorPosition = p;
}

Point3 GlCanvas::getCursorPosition() {
    return _d->cursorPosition;
}

void GlCanvas::setCursorOrientation(Point3 n) {
    _d->cursorOrientation = n;
}

Point3 GlCanvas::getCursorOrientation() {
    return _d->cursorOrientation;
}

IRenderer* GlCanvas::renderer() const {
    return _d->renderer;
}

void GlCanvas::setPerspectiveView(PerspectiveType type) {
    _d->viewType = type;
}

void GlCanvas::setGridVisible(bool value)
{
    _d->isGridVisible = value;
    updateGL();
}

bool GlCanvas::isGridVisible()
{
    return _d->isGridVisible;
}

bool GlCanvas::areNormalsVisible()
{
    return _d->areNormalsVisible;
}

void GlCanvas::setNormalsVisible(bool visible)
{
    _d->areNormalsVisible = visible;
}

void GlCanvas::setDrawingMode(DrawingMode mode){
    _d->drawingMode = mode;
    delete _d->renderer;
    _d->renderer = RendererFactory::getRenderer(_d->drawingMode);
}

void GlCanvas::initializeGL()
{
    // Set up the rendering context, define display lists etc.:

    //glClearColor( 0.4, 0.4, 0.4, 1.0 );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    enable( GL_DEPTH_TEST);
    enable( GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    disable(GL_LIGHTING);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
        qDebug("Depth buffer not enabled.");
    else
    {
        qDebug("Depth buffer enabled. Precision: %d", format().depthBufferSize());
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    qDebug() << "hasOGL Shaders: " << QGLShaderProgram::hasOpenGLShaderPrograms();
    g_shaderProgram = new GlslProgram();
    g_shaderVertex = new VertexShader();
    g_shaderFragment = new FragmentShader();

    g_shaderProgram->attachShader(g_shaderVertex);
    g_shaderProgram->attachShader(g_shaderFragment);

    QString path = g_pApp->applicationDirPath();
    try {
        QString filename = path + QString("/../Resources/shaders/phong.vs");
        if (!g_shaderVertex->loadFromFile(filename.toStdString())) {
            qDebug() << "Failed to load file";
        } else if (!g_shaderVertex->compile()) {
            qDebug() << "Failed to compile shader";
            qDebug() << g_shaderVertex->infoLog().c_str();
        }
        filename = path + QString("/../Resources/shaders/phong.fs");
        if (!g_shaderFragment->loadFromFile(filename.toStdString())) {
            qDebug() << "Failed to load file";
        } if (!g_shaderFragment->compile()) {
            qDebug() << "Failed to compile shader";
            qDebug() << g_shaderFragment->infoLog().c_str();
        }
        
        if (g_shaderVertex->compileStatus() && 
            g_shaderFragment->compileStatus() &&
            !g_shaderProgram->link()) {
            qDebug() << "Failed to link shaders";
            qDebug() << g_shaderProgram->buildLog().c_str();
        }
    } catch (core::GlException & e) {
        std::cerr   << "Failed to compile shaders: " << e.what() 
                    << " error " << e.error()
                    << ": " << e.errorString() << std::endl;
    }
}

void GlCanvas::resizeGL( int w, int h )
{
    if ( h > 0 )
        _d->aspectRatio = GLdouble( w ) / GLdouble( h );

    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
             DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
             -DEFAULT_HEIGHT / 2 * _d->zoomFactor,
             DEFAULT_HEIGHT / 2 * _d->zoomFactor,
             -1000.0,
             1000.0 );

    glMatrixMode( GL_MODELVIEW );

    glGetIntegerv(GL_VIEWPORT, _d->viewport);

    // Now update the cameras to have the same viewport and projection
    // information
    foreach(Camera* c, _d->cameraList) {
        c->setViewport(0, 0, w, h);
        c->setOrthoMatrix( -DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
                           DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
                           -DEFAULT_HEIGHT / 2 * _d->zoomFactor,
                           DEFAULT_HEIGHT / 2 * _d->zoomFactor,
                           -1000.0,
                           1000.0 );
    }
}

void GlCanvas::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindTexture(GL_TEXTURE_2D, 0);
    glLoadIdentity();

    Camera* camera = _d->cameraList.contains(_d->viewType) ? _d->cameraList[_d->viewType] : NULL;
    if (camera)
    {
        glLoadMatrixf(camera->modelView().data());
    }
    
    //glTranslatef(-100, -100, 0);

    if (_d->isGridVisible)
        drawGrid();

    glLineWidth(1);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    enable(GL_BLEND);
    enable(GL_LINE_SMOOTH);

    glBegin( GL_LINES );
    glColor3f( 1.0, 0.0, 0.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 1.0f, 0.0f, 0.0f );

    glColor3f( 0.0, 1.0, 0.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 1.0f, 0.0f );

    glColor3f( 0.0, 0.0, 1.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 1.0f );
    glEnd();

    Eigen::Vector4f eyePosition, lightPosition(-1, 2, -2, 1);
    if (camera) {
        eyePosition = camera->getPosition().homogeneous();
        lightPosition = eyePosition;
    }

    uint loc;
    try {
        g_shaderProgram->useProgram();
        loc = g_shaderProgram->uniformLocation("eyePosition");
        g_shaderProgram->setUniform(loc, eyePosition);
        loc = g_shaderProgram->uniformLocation("lightPosition");
        g_shaderProgram->setUniform(loc, lightPosition);
        loc = g_shaderProgram->uniformLocation("diffuseColor");
        g_shaderProgram->setUniform(loc, Eigen::Vector4f(0.5f, 0.2f, 1.0f, 1.0f));
        loc = g_shaderProgram->uniformLocation("specularColor");
        g_shaderProgram->setUniform(loc, Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        loc = g_shaderProgram->uniformLocation("exponent");
        g_shaderProgram->setUniform(loc, 220.0f);
    } catch (core::GlException & e) {
        qDebug() << "GlException - " << e.what();
        qDebug() << "Error " << e.error() << ": " << e.errorString();
    }
    
    auto ptr = g_pApp->getMainWindow()->getCurrentDocument()->scene().lock();
    drawScene(ptr);
    g_shaderProgram->releaseProgram();

//    drawCursor();

    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();
    if (cmd) {
        cmd->paintGL(this);
    }

    glFlush();
}

void GlCanvas::drawScene(Scene::shared_ptr scene)
{
    if (_d->renderer == NULL)
        return;
    
    Iterator<SceneNode> it = scene->iterator();
    while(it.hasNext())
    {
        glPushMatrix();
        auto n = it.next();
        glMultMatrixf(n->transform().data());
        drawSceneNode(n);
        glPopMatrix();
    }
}

void GlCanvas::drawSceneNode(SceneNode::shared_ptr node)
{
    ISurface* mesh;
    SurfaceNode::shared_ptr s;
    
    if (node->nodeType() == NT_Surface)
    {
        s = std::dynamic_pointer_cast<SurfaceNode>(node);
        mesh = s->surface();
        if (node->isSelected()) {
            g_shaderProgram->releaseProgram();
            drawBoundingBox(mesh);
            g_shaderProgram->useProgram();
        }
        _d->renderer->renderObject(mesh);
    }
    
    Iterator<SceneNode> it = node->iterator();
    while(it.hasNext())
    {
        glPushMatrix();
        auto n = it.next();
        glMultMatrixf(n->transform().data());
        drawSceneNode(n);
        glPopMatrix();
    }
}

void GlCanvas::drawBoundingBox(const ISurface* mesh)
{
    using namespace geometry;

    glColor3f(0.0, 1.0, 0.0);
    
    AABB bb = mesh->boundingBox();
    float minX = bb.min().x() - 0.1;
    float minY = bb.min().y() - 0.1;
    float minZ = bb.min().z() - 0.1;
    float maxX = bb.max().x() + 0.1;
    float maxY = bb.max().y() + 0.1;
    float maxZ = bb.max().z() + 0.1;
    
    glColor3f(0.0, 1.0, 0.0);

    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, minZ);
    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, maxY, minZ);
    glVertex3f(minX, maxY, minZ);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ);
    glVertex3f(minX, minY, maxZ);

    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, minY, maxZ);

    glVertex3f(maxX, maxY, minZ);
    glVertex3f(maxX, maxY, maxZ);

    glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();
}

void GlCanvas::drawGrid()
{
    const double GRID_PLANE_Z = 0.0;
    glLineWidth(0.5);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    enable(GL_BLEND);
    enable(GL_LINE_SMOOTH);
    disable(GL_DEPTH_TEST);

    glBegin(GL_LINES);

    glColor3f(0.3, 0.3, 0.3);
    for (double j = -5.0; j < 5.0; j+=0.2)
    {
        if ( int((j+50.0) * 10) % 10 == 0)
            continue;
        glVertex3f( j, -5.0, GRID_PLANE_Z);
        glVertex3f( j,  5.0, GRID_PLANE_Z);

        glVertex3f(-5.0, j, GRID_PLANE_Z);
        glVertex3f( 5.0, j, GRID_PLANE_Z);
    }
    glEnd();

    glLineWidth(2.0);

    glBegin(GL_LINES);
    glColor3f(0.25, 0.25, 0.25);
    for (double i = -5.0; i <= 5.0; i += 1.0)
    {
        glVertex3f( i, -5.0, GRID_PLANE_Z);
        glVertex3f( i,  5.0, GRID_PLANE_Z);

        glVertex3f(-5.0, i, GRID_PLANE_Z);
        glVertex3f( 5.0, i, GRID_PLANE_Z);
    }
    glEnd();
    glLineWidth(1.0);
    enable(GL_DEPTH_TEST);
}

void GlCanvas::drawCursor()
{
    if (_d->cursorShape == None)
        return;

    switch(_d->cursorShape)
    {
    case None:
    case Cross:
    case Circle:
        break;
    case Image:
        {
            // Store current state of feature that we could modify
            bool blendState = glIsEnabled(GL_BLEND);
            bool textureState = glIsEnabled(GL_TEXTURE_2D);
            bool depthTestState = glIsEnabled(GL_DEPTH_TEST);

            // Enable the texture and set the blend parameters
            enable(GL_TEXTURE_2D);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            enable(GL_BLEND);

            // Disable depth test so we dont modify previusly rendered information
            if (depthTestState)
                glDisable( GL_DEPTH_TEST);

            // Store the current matrix to restore it later
            glPushMatrix();

            // Start from a clean transformation matrix
            glLoadIdentity();

            // Setup the texture and color
            glBindTexture(GL_TEXTURE_2D, _d->textureId);
            glColor3f(1.0f, 1.0f, 0.0f);

            // Calculate the coordinates of the box to paint the bitmap
            double wx1, wy1, wz1;
            screenToWorld(_d->cursorPosition.x() - _d->cursorImage.width()/2,
                                         _d->cursorPosition.y() - _d->cursorImage.height() / 2,
                                         _d->cursorPosition.z(), &wx1, &wy1, &wz1);
            double wx2, wy2, wz2;
            screenToWorld(_d->cursorPosition.x() + _d->cursorImage.width()/2,
                                         _d->cursorPosition.y() + _d->cursorImage.height() / 2,
                                         _d->cursorPosition.z(), &wx2, &wy2, &wz2);

            // Draw Bitmap cursor as a textured quad
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(wx1, wy1, wz1);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(wx1, wy2, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(wx2, wy2, 0.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(wx2, wy1, 0.0f);
            glEnd();
            glPopMatrix();

            // Restore previus states
            if (blendState == false)
                disable(GL_BLEND);
            if (textureState == false)
                disable(GL_TEXTURE_2D);
            if (depthTestState)
                disable( GL_DEPTH_TEST);
        }
        break;
    }
}

void GlCanvas::drawOrientationAxis()
{
    // setup viewport, projection etc.:
    glViewport( 0, 0, 100, 100 );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( -1.5 * _d->aspectRatio, 1.5 * _d->aspectRatio, 1.5, -1.5, 2.0, -2.0 );


    glMatrixMode( GL_MODELVIEW );

    Camera* camera = _d->cameraList.contains(_d->viewType) ? _d->cameraList[_d->viewType] : NULL;
    if (camera)
    {
        gluLookAt( camera->getPosition().x(), camera->getPosition().y(),
                   camera->getPosition().z(), camera->getTargetPoint().x(),
                   camera->getTargetPoint().y(), camera->getTargetPoint().z(),
                   camera->getOrientationVector().x(),
                   camera->getOrientationVector().y(),
                   camera->getOrientationVector().z());
        //qDebug(qPrintable(camera->toString()));
    }

    glBegin( GL_LINES );
    glColor3f( 1.0, 0.0, 0.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 1.0f, 0.0f, 0.0f );

    glColor3f( 0.0, 1.0, 0.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 1.0f, 0.0f );

    glColor3f( 0.0, 0.0, 1.0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 1.0f );
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glViewport( _d->viewport[0], _d->viewport[1], _d->viewport[2], _d->viewport[3] );
}

void GlCanvas::mouseMoveEvent ( QMouseEvent * e )
{
    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();

    bool needUpdate = false;
    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
        //this->getViewCamera()->
    }
    else
    {
        if (cmd)
        {
            cmd->mouseMoveEvent( e );
            needUpdate = true;
        }
        if (_d->cursorShape != None)
        {
            //_cursorPosition.setPoint(e->x(), e->y(), 0);
            _d->cursorPosition.x() = e->x();
            _d->cursorPosition.y() = e->y();
            _d->cursorPosition.z() = 0;
            needUpdate = true;
        }
    }

    if (needUpdate)
    {
        updateGL();
    }
}

void GlCanvas::mousePressEvent ( QMouseEvent * e )
{
    //qDebug("MousePress");
    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();
    bool needUpdate = false;

    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
    }
    else
    {
        if (cmd)
        {
            cmd->mousePressEvent( e );
            needUpdate = true;
        }
        _d->cursorPosition.x() = e->x();
        _d->cursorPosition.y() = e->y();
        if (_d->cursorShape != None)
        {
            _d->cursorPosition.x() = e->x();
            _d->cursorPosition.y() = e->y();
            needUpdate = true;
        }
    }
    if (needUpdate)
    {
        updateGL();
    }
}

void GlCanvas::mouseReleaseEvent ( QMouseEvent * e )
{
    //qDebug("MouseRelease");
    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();

    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
    }
    else
    {
        if (cmd)
        {
            cmd->mouseReleaseEvent( e );
            updateGL();
        }
    }
}

void GlCanvas::wheelEvent ( QWheelEvent * e )
{
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;

    _d->zoomFactor += numSteps * 0.01;

    resizeGL( width(), height() );
    updateGL();
}

ObjectContainer GlCanvas::getSelectedObjects(GLint x, GLint y)
{
    ObjectContainer l;
    if (_d->renderer == NULL)
        return l;

    unsigned int objId = 1;
    GLuint d = 0;
    ISurface* mesh;
    auto doc= ((DocumentView*)parentWidget())->getDocument()->scene().lock();
    if (!doc) {
        return l;
    }
    
    Iterator<SceneNode> it = doc->iterator();
    while(it.hasNext()) {
        glPushMatrix();
        auto n = it.next();
        SurfaceNode::shared_ptr s;
        if (n->nodeType() == NT_Surface) {
            s = std::dynamic_pointer_cast<SurfaceNode> (n);
            mesh = s->surface();
            Point3 p = s->localToWorld(Point3(0,0,0));
            glTranslatef(p.x(), p.y(), p.z());
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_picking.renderObject(mesh, objId);
            glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&d);
            
            if (d == objId)
                l.append(mesh);
            
            objId++;
        }
        glPopMatrix();
    }

    return l;
}

PointIndexList GlCanvas::getSelectedVertices(GLint x, GLint y,
                                             GLint width, GLint height)
{
    PointIndexList l;
    if (_d->renderer == NULL || width == 0 || height == 0)
        return l;

    int halfWidth = width / 2;
    int halfHeight = height / 2;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    unsigned int objId = 1;
    GLuint* d = new GLuint[width * height];
    ISurface* mesh;
    
    auto doc= ((DocumentView*)parentWidget())->getDocument()->scene().lock();
    if (!doc) {
        return l;
    }
    Iterator<SceneNode> it = doc->iterator();
    while(it.hasNext()) {
        memset(d, 0, width*height*sizeof(GLuint));
        glPushMatrix();
        auto n = it.next();
        SurfaceNode::shared_ptr s;
        if (n->nodeType() == NT_Surface) {
            s = std::dynamic_pointer_cast<SurfaceNode>(n);
            mesh = s->surface();
            Point3 p = s->localToWorld(Point3(0,0,0));
            glTranslatef(p.x(), p.y(), p.z());
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_pickingVertices.renderObject(mesh, objId);
            glReadPixels(x - halfWidth, _d->viewport[3]- (y + halfHeight),
                         width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)d);
            
            int faceIndex = 0;
            for (int j = 0; j < width * height; ++j)
            {
                if (d[j] == 0) continue;

                faceIndex = d[j]-1;
                Face* f = mesh->face(faceIndex);
                
                Iterator<Vertex> vtxIt = f->constVertexIterator();
                while(vtxIt.hasNext()) {
                    //const Vertex& v = vtxIt.next();
                }

            }
            
            objId++;
        }
        glPopMatrix();
    }    

    delete [] d;
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    return l;
}

Camera* GlCanvas::getViewCamera()
{
    if (!_d->cameraList.contains(_d->viewType))
    {
        //qDebug("camera %d does not exists. Returning perspective camera.", _d->viewType);
        return _d->cameraList[Perspective];
    }
    return _d->cameraList[_d->viewType];
}

const Camera* GlCanvas::getViewCamera() const
{
    if (!_d->cameraList.contains(_d->viewType))
    {
        //qDebug("camera %d does not exists. Returning perspective camera.", _d->viewType);
        return _d->cameraList[Perspective];
    }
    return _d->cameraList[_d->viewType];
}

void GlCanvas::set3DCursorShape(CursorShapeType shape)
{
    _d->cursorShape = shape;

    // Turn on mouse tracking only if we are going to draw a cursor
    setMouseTracking(_d->cursorShape != None);
}

void GlCanvas::setCursorImage(const QImage& image)
{
    if (image.isNull())
    {
        qDebug("GlDisplay::setCursorImage: image is null !!!");
    }
    else
    {
        // Delete any texture loaded before.
        deleteTexture(_d->textureId);
        _d->textureId = 0;

        _d->cursorImage = image;
        _d->textureId = bindTexture(_d->cursorImage);
        if (_d->textureId == 0)
        {
            qDebug("texture id is not valid");
        }
    }

}

/**
 * Returns a copy of the image used as cursor.
 */
QImage GlCanvas::getCursorImage()
{
    return _d->cursorImage;
}

void GlCanvas::screenToWorld(double x, double y, double z,
                            double *wx, double *wy, double *wz) const
{
    Point3 p(x, y, z);
    Point3 o = screenToWorld(p);
    if (wx) *wx = o.x();
    if (wy) *wy = o.y();
    if (wz) *wz = o.z();
}

void GlCanvas::screenToWorld(const Point3& winCoords,
                            Point3& world) const
{
    world = screenToWorld(winCoords);
}

Point3 GlCanvas::screenToWorld(const Point3& p) const
{
    return getViewCamera()->eyeToWorld(p);
}

void GlCanvas::worldToScreen(double wx, double wy, double wz,
                            double *x, double *y, double *z) const
{
    Point3 p(wx, wy, wz);
    Point3 o = worldToScreen(p);
    if (x) *x = o.x();
    if (y) *y = o.y();
    if (z) *z = o.z();
}

void GlCanvas::worldToScreen(const Point3& world,
                            Point3& winCoords) const
{
    winCoords = worldToScreen(world);
}

Point3 GlCanvas::worldToScreen(const Point3& p) const
{
    return getViewCamera()->worldToEye(p);
}

/**
 *
 */
void GlCanvas::setPen(const QPen& pen)
{
    _d->pen = pen;
}

/**
 *
 */
const QPen& GlCanvas::pen() const
{
    return _d->pen;
}

/**
 *
 */
void GlCanvas::setBrush(const QBrush& brush)
{
    _d->brush = brush;
}

/**
 *
 */
const QBrush& GlCanvas::brush() const
{
    return _d->brush;
}


Color GlCanvas::color(const Point3 & p)
{
    GLint x = p[0];
    GLint y = p[1];
    return color(x, y);
}

Color GlCanvas::color(int x, int y)
{    
    return Color::fromUintRGBA(colorUint(x, y));
}

uint GlCanvas::colorUint(const Point3 & p)
{
    GLint x = p[0];
    GLint y = p[1];
    return colorUint(x, y);
}


uint GlCanvas::colorUint(int x, int y)
{
    GLuint d;
    
    glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&d);
    
    return d;
}

float GlCanvas::depth(const Point3 & p)
{
    GLint x = p[0];
    GLint y = p[1];
    return depth(x, y);
}

float GlCanvas::depth(int x, int y)
{
    float d;    
    
    glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);
    return d;
}

void GlCanvas::begin(GLenum mode)
{
    glBegin(mode);
    _d->flags |= GLCANVAS_BEGIN;
}

void GlCanvas::end()
{
    _d->flags &= ~GLCANVAS_BEGIN;
    glEnd();
}


void GlCanvas::drawLine(const Point3& p1, const Point3& p2)
{
    double c[4];
    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0) {
        glColor4dv(c);
        if (!_d->flags & GLCANVAS_BEGIN) glBegin(GL_LINES);
        glVertex3fv(p1.data());
        glVertex3fv(p2.data());
        if (!_d->flags & GLCANVAS_BEGIN) glEnd();
    }
}

void GlCanvas::drawLine2D(const Point3& p1, const Point3& p2)
{
    double c[4];
    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0) {
        Point3 tmp1, tmp2;
        screenToWorld(p1, tmp1);
        screenToWorld(p2, tmp2);
        glColor4dv(c);
        if (!_d->flags & GLCANVAS_BEGIN) glBegin(GL_LINES);
        glVertex3fv(tmp1.data());
        glVertex3fv(tmp2.data());
        if (!_d->flags & GLCANVAS_BEGIN) glEnd();
    }
}

void GlCanvas::drawRect(const Point3& p1, const Point3& p2, int mode)
{
    switch(mode) {
    case 0:
        drawRectWinCoord(p1, p2);
        break;
    }
}

void GlCanvas::drawRectWinCoord(const Point3& c1, const Point3& c2)
{
    double c[4];
    Point3 p1, p2, p3, p4;
    Point3 a1 = c1;
    Point3 a3 = c2;
    Point3 a2 = Point3(a3.x(), a1.y(), a1.z());
    Point3 a4 = Point3(a1.x(), a3.y(), a1.z());
    screenToWorld(a1, p1);
    screenToWorld(a2, p2);
    screenToWorld(a3, p3);
    screenToWorld(a4, p4);
    _d->brush.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0) {
        glColor4dv(c);
        glBegin(GL_QUADS);
        glVertex3fv(p1.data());
        glVertex3fv(p2.data());
        glVertex3fv(p3.data());
        glVertex3fv(p4.data());
        glEnd();
    }

    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0 ) {
        glColor4dv(c);
        glBegin(GL_LINE_LOOP);
        glVertex3fv(p1.data());
        glVertex3fv(p2.data());
        glVertex3fv(p3.data());
        glVertex3fv(p4.data());
        glVertex3fv(p1.data());
        glEnd();
    }
}

/**
 *
 */
void GlCanvas::drawEllipse(const Point3& center, 
                           float axis1,
                           float axis2,
                           float innerAxis1,
                           float innerAxis2)
{
    drawEllipseWinCoord(center, 0, 360, axis1,axis2,innerAxis1,innerAxis2);
}

void GlCanvas::drawArc(const Point3& center, 
             float startAngle,
             float endAngle,
             float axis1,
             float axis2,
             float innerAxis1,
             float innerAxis2)
{
    drawEllipseWinCoord(center,startAngle, endAngle, 
                        axis1,axis2,innerAxis1,innerAxis2);
}

// Helper function to compute the next value in an ellipse
static void calculateEllipse(double x, double y, double a, double b,
                             double startAngle, double endAngle, double angle,
                             int steps, std::vector<Point3>& points) 
{
    if (steps == 0)
        steps = 36;
    
    points.clear();
    points.reserve(steps+1);
    
    double deltaAngle = endAngle - startAngle;
    // Angle is given by Degree Value
    double radFactor = M_PI / 180; // factor to convert to radians
    double beta = angle * radFactor; // converts Degree Value into Radians
    double sinbeta = sin(beta);
    double cosbeta = cos(beta);
    double angleStep = deltaAngle / steps * radFactor;
    double alpha = startAngle * radFactor;
    for (int j=0; j <= steps; ++j)
    {
        double sinalpha = sin(alpha);
        double cosalpha = cos(alpha);
        
        double X = x + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
        double Y = y + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);
        
        points.push_back(Point3(X, Y, 0));
        
        alpha += angleStep;
    }
}

void GlCanvas::drawEllipseWinCoord(const Point3& center,
                                   float startAngle,
                                   float endAngle,
                                   float axis1,
                                   float axis2,
                                   float innerAxis1,
                                   float innerAxis2)
{
    const int NUM_POINTS = 36;
    std::vector<Point3> points;
    std::vector<Point3> innerPoints;
    bool drawInnerEllipse = innerAxis1 != 0 && innerAxis2 != 0;
    
    calculateEllipse(center.x(), center.y(), axis1, axis2, 
                     0, endAngle, startAngle, NUM_POINTS, points);
    if (drawInnerEllipse) {
        calculateEllipse(center.x(), center.y(), innerAxis1, innerAxis2,
                         0, endAngle, startAngle, NUM_POINTS, innerPoints);
    }
    int npoints = points.size();
    for (int i = 0; i < npoints; ++i) {
        screenToWorld(points[i], points[i]);
        if (drawInnerEllipse) {
            screenToWorld(innerPoints[i], innerPoints[i]);
        }
    }
    
    double color[4];
    _d->brush.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    glColor4dv(color);
    glBegin(GL_TRIANGLES);
    if (drawInnerEllipse) {
        for (int i = 0; i < npoints - 1; ++i) {
            glVertex3fv(points[i].data());
            glVertex3fv(innerPoints[i].data());
            glVertex3fv(points[i+1].data());
            glVertex3fv(innerPoints[i].data());
            glVertex3fv(points[i+1].data());
            glVertex3fv(innerPoints[i+1].data());
        }
    }
    else {
        Point3 c;
        screenToWorld(center, c);
        for (int i = 0; i < npoints - 1; ++i) {
            glVertex3fv(points[i].data());
            glVertex3fv(c.data());
            glVertex3fv(points[i+1].data());
        }
    }
    glEnd();
    
    _d->pen.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    glColor4dv(color);
    // draw main ellipse
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < npoints; ++i) {
        glVertex3fv(points[i].data());
    }
    glEnd();
    
    // draw inner ellipse
    if (drawInnerEllipse) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < npoints; ++i) {
            glVertex3fv(innerPoints[i].data());
        }
        glEnd();
    }
}
