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
#include <QtOpenGL>
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtAlgorithms>
#include <QLineF>
#include <QRectF>
#include <QPolygonF>
#include <QMatrix>
#include <QBrush>
#include <QPen>
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

PickingObjectRenderer g_picking;
PickingFacesRenderer g_pickingVertices;

#define SELECT_BUFFER_SIZE 512
#define DEFAULT_HEIGHT 5.0f

QGLShaderProgram    *g_shaderProgram = NULL;
QGLShader           *g_shaderVertex = NULL;
QGLShader           *g_shaderFragment = NULL;

struct Rect {
    GLfloat x1, y1, x2, y2;

    Rect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {
    }
};


GlCanvas::GlCanvas(DocumentView* _parent)
    : QGLWidget(_parent),
    _isGridVisible(false),
    _areNormalsVisible(false),
    _selectBuffer(NULL),
    _aspectRatio(1.0),
    _viewType(Front),
    _drawingMode(Points),
    _renderer(NULL),
    _selectionRenderer(NULL),
    _editVertexRenderer(NULL),
    _cursorShape(None),
    _zoomFactor(1.0),
    _textureId(0),
    _pen(QPen(QColor(Qt::white))),
    _brush(QBrush(Qt::transparent))
{
    // Type of renderer used for displaying objects on the screen
    _renderer = RendererFactory::getRenderer(_drawingMode);

    // Type of renderer used for user object picking
    _selectionRenderer = RendererFactory::getRenderer(Points);

    // Type of renderer used for vertex edition mode
    _editVertexRenderer = RendererFactory::getRenderer(Points);

    _selectBuffer = new GLuint[SELECT_BUFFER_SIZE];

    Camera* camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector(Point3( 0, 1, 0) );
    camera->setPosition( Point3( 0, 0, 1));
    _cameraList[Front] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 1, 0) );
    camera->setPosition( Point3( 0, 0, -1));
    _cameraList[Back] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 0, -1) );
    camera->setPosition( Point3( 0, 1, 0) );
    _cameraList[Top] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 0, 1) );
    camera->setPosition( Point3( 0, -1, 0) );
    _cameraList[Bottom] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 1, 0) );
    camera->setPosition( Point3(-1, 0, 0) );
    _cameraList[Left] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 1, 0) );
    camera->setPosition( Point3( 1, 0, 0) );
    _cameraList[Right] = camera;

    camera = new Camera();
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 0, 1) );
    camera->setPosition( Point3( 0.75, 0.75, 0.75) );
    _cameraList[Perspective] = camera;

    setCursor(Qt::CrossCursor);
}


GlCanvas::~GlCanvas()
{
    if (_selectBuffer)
        delete [] _selectBuffer;

    QMutableMapIterator<int, Camera*> it(_cameraList);
    while(it.hasNext())
    {
        it.next();
        delete it.value();
        it.remove();
    }

    delete _renderer;
    _renderer = NULL;

    delete _selectionRenderer;
    _selectionRenderer = NULL;
}

void GlCanvas::setGridVisible(bool value)
{
    _isGridVisible = value;
    updateGL();
}

bool GlCanvas::isGridVisible()
{
    return _isGridVisible;
}

bool GlCanvas::areNormalsVisible()
{
    return _areNormalsVisible;
}

void GlCanvas::setNormalsVisible(bool visible)
{
    _areNormalsVisible = visible;
}

void GlCanvas::setDrawingMode(DrawingMode mode){
    _drawingMode = mode;
    delete _renderer;
    _renderer = RendererFactory::getRenderer(_drawingMode);
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
    glShadeModel(GL_SMOOTH);
    enable(GL_LIGHT0);
    glSelectBuffer(SELECT_BUFFER_SIZE, _selectBuffer);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
        qDebug("Depth buffer not enabled.");
    else
    {
        qDebug("Depth buffer enabled. Precision: %d", format().depthBufferSize());
    }

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    enable(GL_COLOR_MATERIAL);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    qDebug() << "hasOGL Shaders: " << QGLShaderProgram::hasOpenGLShaderPrograms();
    g_shaderProgram = new QGLShaderProgram(this);
    g_shaderVertex = new QGLShader(QGLShader::Vertex, this);
    g_shaderFragment = new QGLShader(QGLShader::Fragment, this);

    g_shaderProgram->addShader(g_shaderVertex);
    g_shaderProgram->addShader(g_shaderFragment);

    QString path = g_pApp->applicationDirPath();

    qDebug() << "shaderId: " << g_shaderVertex->shaderId();
    if (!printGlError()) qDebug() << __LINE__;
    do {
        if (!g_shaderVertex->compileSourceFile(":shaders/phong.vs"))
        {
            qDebug() << "Failed to compile vertex shader";
            qDebug() << g_shaderVertex->log();
            break;
        }
        if (!printGlError()) qDebug() << __LINE__;
        if (!g_shaderFragment->compileSourceFile(":shaders/phong.fs"))
        {
            qDebug() << "Failed to compile fragment shader";
            qDebug() << g_shaderFragment->log();
            break;
        }
        if (!printGlError()) qDebug() << __LINE__;
        if (!g_shaderProgram->link())
        {
            qDebug() << "Failed to link shaders";
            qDebug() << g_shaderProgram->log();
            break;
        }
        if (!printGlError()) qDebug() << __LINE__;
    } while (false);
}

void GlCanvas::resizeGL( int w, int h )
{
    if ( h > 0 )
        _aspectRatio = GLdouble( w ) / GLdouble( h );

    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -DEFAULT_HEIGHT / 2 * _zoomFactor * _aspectRatio,
             DEFAULT_HEIGHT / 2 * _zoomFactor * _aspectRatio,
             -DEFAULT_HEIGHT / 2 * _zoomFactor,
             DEFAULT_HEIGHT / 2 * _zoomFactor,
             -1000.0,
             1000.0 );

    glMatrixMode( GL_MODELVIEW );

    glGetIntegerv(GL_VIEWPORT, _viewport);

    // Now update the cameras to have the same viewport and projection
    // information
    foreach(Camera* c, _cameraList) {
        c->setViewport(0, 0, w, h);
        c->setOrthoMatrix( -DEFAULT_HEIGHT / 2 * _zoomFactor * _aspectRatio,
                           DEFAULT_HEIGHT / 2 * _zoomFactor * _aspectRatio,
                           -DEFAULT_HEIGHT / 2 * _zoomFactor,
                           DEFAULT_HEIGHT / 2 * _zoomFactor,
                           -1000.0,
                           1000.0 );
    }
}

void GlCanvas::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindTexture(GL_TEXTURE_2D, 0);
    glLoadIdentity();

    Camera* camera = _cameraList.contains(_viewType) ? _cameraList[_viewType] : NULL;
    if (camera)
    {
        glLoadMatrixf(camera->modelView().data());
    }
    
    //glTranslatef(-100, -100, 0);
    glDisable(GL_LIGHTING);

    if (_isGridVisible)
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

    switch(_drawingMode)
    {
    case Smooth:
    case Wireframe:
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        break;
    case Flat:
        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
        break;
    case Texture:
    case Points:
        break;
    }

    Eigen::Vector4f eyePosition, lightPosition(-1, 2, -2, 1);
    if (camera) {
        eyePosition.x() = camera->getPosition().x();
        eyePosition.y() = camera->getPosition().y();
        eyePosition.z() = camera->getPosition().z();
        eyePosition.w() = 1.0f;
    }

//    g_shaderProgram->bind();
//    printGlError();
//    uint loc = g_shaderProgram->uniformLocation("eyePosition");
//    g_shaderProgram->setUniformValue(loc, eyePosition.x(),
//                                     eyePosition.y(), eyePosition.z(),
//                                     eyePosition.w());
//    printGlError();
//    loc = g_shaderProgram->uniformLocation("lightPosition");
//    g_shaderProgram->setUniformValue(loc, lightPosition.x(),
//                                     lightPosition.y(), lightPosition.z(),
//                                     lightPosition.w());
//    printGlError();
//    loc = g_shaderProgram->uniformLocation("diffuseColor");
//    g_shaderProgram->setUniformValue(loc, 0.5f, 0.2f, 1.0f, 1.0f);
//    printGlError();
//    loc = g_shaderProgram->uniformLocation("specularColor");
//    g_shaderProgram->setUniformValue(loc, 1.0f, 1.0f, 1.0f, 1.0f);
//    printGlError();
//    loc = g_shaderProgram->uniformLocation("exponent");
//    g_shaderProgram->setUniformValue(loc, 220.0f);
//    printGlError();

    drawObjects();
//    g_shaderProgram->release();

    drawCursor();

    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();
    if (cmd) {
        cmd->paintGL(this);
    }

    glFlush();
}

void GlCanvas::drawObjects()
{
    if (_renderer == NULL)
        return;

    ISurface* mesh;
    IDocument* doc= g_pApp->getMainWindow()->getCurrentDocument();
    
    Iterator<SceneNode> it = doc->scene()->iterator();
    SurfaceNode *s;
    while(it.hasNext())
    {
        glPushMatrix();
        SceneNode *n = &it.next();
        if (n->nodeType() == NT_Surface)
        {
            s = static_cast<SurfaceNode*> (n);
            mesh = s->surface();
            
            float x = 0.0f, y = 0.0f, z = 0.0f;
            mesh->getPosition(&x, &y, &z);
            glTranslatef(x, y, z);
            mesh->orientation(x, y, z);
            glRotatef(x, 1, 0, 0);
            glRotatef(y, 0, 1, 0);
            glRotatef(z, 0, 0, 1);
            if (mesh->isSelected()) {
                drawBoundingBox(mesh);
            }
            _renderer->renderObject(mesh);
        }
        else {
            glMultMatrixf(n->transform().data());
        }

        glPopMatrix();
    }
}

void GlCanvas::drawBoundingBox(const ISurface* mesh)
{
    using namespace geometry;
    
    GLboolean lightEnabled = GL_FALSE;
    lightEnabled = glIsEnabled(GL_LIGHTING);

    if (lightEnabled == GL_TRUE)
        glDisable(GL_LIGHTING);

    glColor3f(0.0, 1.0, 0.0);
    
    AABB bb = mesh->getBoundingBox();
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

    if (lightEnabled == GL_TRUE)
        glEnable(GL_LIGHTING);
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
    if (_cursorShape == None)
        return;

    switch(_cursorShape)
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
            glBindTexture(GL_TEXTURE_2D, _textureId);
            glColor3f(1.0f, 1.0f, 0.0f);

            // Calculate the coordinates of the box to paint the bitmap
            double wx1, wy1, wz1;
            mapScreenCoordsToWorldCoords(_cursorPosition.x() - _cursorImage.width()/2,
                                         _cursorPosition.y() - _cursorImage.height() / 2,
                                         _cursorPosition.z(), &wx1, &wy1, &wz1);
            double wx2, wy2, wz2;
            mapScreenCoordsToWorldCoords(_cursorPosition.x() + _cursorImage.width()/2,
                                         _cursorPosition.y() + _cursorImage.height() / 2,
                                         _cursorPosition.z(), &wx2, &wy2, &wz2);

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
    bool lightEnabled = glIsEnabled(GL_LIGHTING);

    if (lightEnabled)
        glDisable(GL_LIGHTING);

    // setup viewport, projection etc.:
    glViewport( 0, 0, 100, 100 );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( -1.5 * _aspectRatio, 1.5 * _aspectRatio, 1.5, -1.5, 2.0, -2.0 );


    glMatrixMode( GL_MODELVIEW );

    Camera* camera = _cameraList.contains(_viewType) ? _cameraList[_viewType] : NULL;
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

    glViewport( _viewport[0], _viewport[1], _viewport[2], _viewport[3] );

    if (lightEnabled)
        glEnable(GL_LIGHTING);
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
        if (_cursorShape != None)
        {
            //_cursorPosition.setPoint(e->x(), e->y(), 0);
            _cursorPosition.x() = e->x();
            _cursorPosition.y() = e->y();
            _cursorPosition.z() = 0;
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
        _cursorPosition.x() = e->x();
        _cursorPosition.y() = e->y();
        if (_cursorShape != None)
        {
            _cursorPosition.x() = e->x();
            _cursorPosition.y() = e->y();
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

    _zoomFactor += numSteps * 0.01;

    resizeGL( width(), height() );
    updateGL();
}

ObjectContainer GlCanvas::getSelectedObjects(GLint x, GLint y)
{
    ObjectContainer l;
    if (_renderer == NULL)
        return l;

    unsigned int objId = 1;
    GLuint d = 0;
    ISurface* mesh;
    Scene* doc= ((DocumentView*)parentWidget())->getDocument()->scene();
    Iterator<SceneNode> it = doc->iterator();
    while(it.hasNext()) {
        glPushMatrix();
        SceneNode* n = &it.next();
        SurfaceNode* s;
        if (n->nodeType() == NT_Surface) {
            s = static_cast<SurfaceNode*> (n);
            mesh = s->surface();
            float px = 0.0f, py = 0.0f, pz = 0.0f;
            mesh->getPosition(&px, &py, &pz);
            glTranslatef(px, py, pz);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_picking.renderObject(mesh, objId);
            glReadPixels(x, _viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&d);
            
            if (d == objId)
                l.append(mesh);
            
            objId++;
        }
        glPopMatrix();
    }
//    int count = doc->getObjectsCount();
//    for ( int i = 0; i < count; i++ )
//    {
//        mesh = doc->getObject(i);
//
//        glPushMatrix();
//        float px = 0.0f, py = 0.0f, pz = 0.0f;
//        mesh->getPosition(&px, &py, &pz);
//        glTranslatef(px, py, pz);
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        g_picking.renderObject(mesh, objId);
//        glReadPixels(x, _viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&d);
//
//        if (d == objId)
//            l.append(mesh);
//
//        objId++;
//
//        glPopMatrix();
//        if (printGlError()) break;
//    }
    return l;
}

PointIndexList GlCanvas::getSelectedVertices(GLint x, GLint y,
                                             GLint width, GLint height)
{
    PointIndexList l;
    if (_renderer == NULL || width == 0 || height == 0)
        return l;

    int halfWidth = width / 2;
    int halfHeight = height / 2;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    unsigned int objId = 1;
    GLuint* d = new GLuint[width * height];
    ISurface* mesh;
    
    Scene* doc= ((DocumentView*)parentWidget())->getDocument()->scene();
    Iterator<SceneNode> it = doc->iterator();
    while(it.hasNext()) {
        memset(d, 0, width*height*sizeof(GLuint));
        glPushMatrix();
        SceneNode* n = &it.next();
        SurfaceNode* s;
        if (n->nodeType() == NT_Surface) {
            s = static_cast<SurfaceNode*> (n);
            mesh = s->surface();
            float px = 0.0f, py = 0.0f, pz = 0.0f;
            mesh->getPosition(&px, &py, &pz);
            glTranslatef(px, py, pz);
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_pickingVertices.renderObject(mesh, objId);
            glReadPixels(x - halfWidth, _viewport[3]- (y + halfHeight),
                         width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)d);
            
            int faceIndex = 0;
            for (int j = 0; j < width * height; ++j)
            {
                if (d[j] == 0) continue;

                faceIndex = d[j]-1;
                Face* f = mesh->getFace(faceIndex);
                
                Iterator<Vertex> vtxIt = f->constVertexIterator();
                while(vtxIt.hasNext()) {
                    //const Vertex& v = vtxIt.next();
                }

            }
            
            objId++;
        }
        glPopMatrix();
    }    
//    IDocument* doc= ((DocumentView*)parentWidget())->getDocument();
//    int count = doc->getObjectsCount();
//    for ( int i = 0; i < count; i++ )
//    {
//        mesh = doc->getObject(i);
//
//        memset(d, 0, width*height*sizeof(GLuint));
//        glPushMatrix();
//        float px = 0.0f, py = 0.0f, pz = 0.0f;
//        mesh->getPosition(&px, &py, &pz);
//        glTranslatef(px, py, pz);
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        g_pickingVertices.renderObject(mesh, objId);
//        glReadPixels(x - halfWidth, _viewport[3]- (y + halfHeight),
//                     width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)d);
//
//        int faceIndex = 0;
//        for (int j = 0; j < width * height; ++j)
//        {
//            if (d[j] != 0)
//            {
//                faceIndex = d[j]-1;
//                Face* f = mesh->getFace(faceIndex);
//
//                Iterator<Vertex> vtxIt = f->constVertexIterator();
//                while(vtxIt.hasNext()) {
//                    //const Vertex& v = vtxIt.next();
//                }
//            }
//        }
//
//        objId++;
//
//        glPopMatrix();
//        if (printGlError()) break;
//    }
    delete [] d;
    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    return l;
}

Camera* GlCanvas::getViewCamera()
{
    if (!_cameraList.contains(_viewType))
    {
        //qDebug("camera %d does not exists. Returning perspective camera.", _viewType);
        return _cameraList[Perspective];
    }
    return _cameraList[_viewType];
}

void GlCanvas::set3DCursorShape(CursorShapeType shape)
{
    _cursorShape = shape;

    // Turn on mouse tracking only if we are going to draw a cursor
    setMouseTracking(_cursorShape != None);
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
        deleteTexture(_textureId);
        _textureId = 0;

        _cursorImage = image;
        _textureId = bindTexture(_cursorImage);
        if (_textureId == 0)
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
    return _cursorImage;
}

void GlCanvas::mapScreenCoordsToWorldCoords(double x, double y, double z,
                                            double *wx, double *wy, double *wz)
{
    Point3 p(x, y, z);
    Point3 o = getViewCamera()->eyeToWorld(p);
    if (wx) *wx = o.x();
    if (wy) *wy = o.y();
    if (wz) *wz = o.z();
}

void GlCanvas::mapScreenCoordsToWorldCoords(const Point3& winCoords,
                                            Point3& world)
{
    world = getViewCamera()->eyeToWorld(winCoords);
}

void GlCanvas::mapWorldCoordsToScreenCoords(double wx, double wy, double wz,
                                            double *x, double *y, double *z)
{
    Point3 p(wx, wy, wz);
    Point3 o = getViewCamera()->worldToEye(p);
    if (x) *x = o.x();
    if (y) *y = o.y();
    if (z) *z = o.z();
}

void GlCanvas::mapWorldCoordsToScreenCoords(const Point3& world,
                                            Point3& winCoords)
{
    winCoords = getViewCamera()->worldToEye(world);
}

/**
 *
 */
void GlCanvas::setPen(const QPen& pen)
{
    _pen = pen;
}

/**
 *
 */
const QPen& GlCanvas::pen() const
{
    return _pen;
}

/**
 *
 */
void GlCanvas::setBrush(const QBrush& brush)
{
    _brush = brush;
}

/**
 *
 */
const QBrush& GlCanvas::brush() const
{
    return _brush;
}

/**
 *
 */
void GlCanvas::drawLine(const Point3& p1, const Point3& p2)
{
    double c[4];
    _pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0) {
        glColor4dv(c);
        glBegin(GL_LINES);
        glVertex3fv(p1.data());
        glVertex3fv(p2.data());
        glEnd();
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
    mapScreenCoordsToWorldCoords(a1, p1);
    mapScreenCoordsToWorldCoords(a2, p2);
    mapScreenCoordsToWorldCoords(a3, p3);
    mapScreenCoordsToWorldCoords(a4, p4);
    _brush.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0) {
        glColor4dv(c);
        glBegin(GL_QUADS);
        glVertex3fv(p1.data());
        glVertex3fv(p2.data());
        glVertex3fv(p3.data());
        glVertex3fv(p4.data());
        glEnd();
    }

    _pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
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
    
    double deltaAngle = endAngle - startAngle;
    // Angle is given by Degree Value
    double radFactor = M_PI / 180; // factor to convert to radians
    double beta = -angle * radFactor; // converts Degree Value into Radians
    double sinbeta = sin(beta);
    double cosbeta = cos(beta);
    double angleStep = deltaAngle / steps;
    for (double i = startAngle; i < endAngle + angleStep; i += angleStep) 
    {
        double alpha = i * radFactor ;
        double sinalpha = sin(alpha);
        double cosalpha = cos(alpha);
        
        double X = x + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
        double Y = y + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);
        
        points.push_back(Point3(X, Y, 0));
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
                     startAngle, endAngle, 0, NUM_POINTS, points);
    if (drawInnerEllipse) {
        calculateEllipse(center.x(), center.y(), innerAxis1, innerAxis2,
                         startAngle, endAngle, 0, NUM_POINTS, innerPoints);
    }
    for (int i = 0; i < NUM_POINTS+1; ++i) {
        mapScreenCoordsToWorldCoords(points[i], points[i]);
        if (drawInnerEllipse) {
            mapScreenCoordsToWorldCoords(innerPoints[i], innerPoints[i]);
        }
    }
    
    double color[4];
    _brush.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    glColor4dv(color);
    glBegin(GL_TRIANGLES);
    if (drawInnerEllipse) {
        for (int i = 0; i < NUM_POINTS; ++i) {
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
        mapScreenCoordsToWorldCoords(center, c);
        for (int i = 0; i < NUM_POINTS; ++i) {
            glVertex3fv(points[i].data());
            glVertex3fv(c.data());
            glVertex3fv(points[i+1].data());
        }
    }
    glEnd();
    
    _pen.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    glColor4dv(color);
    // draw main ellipse
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NUM_POINTS; ++i) {
        glVertex3fv(points[i].data());
    }
    glEnd();
    if (drawInnerEllipse) {
        // draw inner ellipse
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < NUM_POINTS; ++i) {
            glVertex3fv(innerPoints[i].data());
        }
        glEnd();
    }
    
    glBegin(GL_POINT);
    
}
