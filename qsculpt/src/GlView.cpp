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
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Picking.h>
#include <PlastilinaCore/PickingFacesRenderer.h>
#include <PlastilinaCore/RendererFactory.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/subdivision/Sphere.h>
#include <QtCore/QLineF>
#include <QtCore/QRectF>
#include <QtCore/QtAlgorithms>
#include <QtGui/QBrush>
#include <QtGui/QMatrix>
#include <QtGui/QMouseEvent>
#include <QtGui/QPen>
#include <QtGui/QPolygonF>
#include <QtGui/QWheelEvent>
#include <iostream>
#include "DocumentView.h"
#include "ICommand.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

PickingObjectRenderer g_picking;
PickingFacesRenderer  g_pickingVertices;

#define SELECT_BUFFER_SIZE 512
#define DEFAULT_HEIGHT 5.0f

GlslProgram*    g_shaderProgram  = NULL;
VertexShader*   g_shaderVertex   = NULL;
FragmentShader* g_shaderFragment = NULL;

struct Rect
{
    GLfloat x1, y1, x2, y2;

    Rect(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
    {
    }
};

#define GLCANVAS_BEGIN (0x0001)

typedef QMap<int, std::shared_ptr<Camera>> CameraContainer;

struct GlCanvas::Impl
{
    bool            isGridVisible;     /**< Grid visibility flag */
    bool            areNormalsVisible; /**< Normals visibility flag */
    GLuint*         selectBuffer;      /**< Selection buffer */
    double          aspectRatio;
    PerspectiveType viewType;    /**< Kind of view to display */
    DrawingMode     drawingMode; /**< Object drawing mode */
    IRenderer*      renderer;    /**< Rendering engine for the objects */
    IRenderer*      selectionRenderer; /**< Renderer used for selection. */
    IRenderer* editVertexRenderer; /**< Renderer used for vertex edition */
    CameraContainer cameraList; /**< Cameras for the differents view types */

    CursorShapeType cursorShape;
    Point3          cursorPosition;
    Point3          cursorOrientation;
    GLint           viewport[4];
    GLfloat         zoomFactor;
    GLuint          textureId;
    QImage          cursorImage;
    QPen            pen;
    QBrush          brush;
    uint32_t        flags;

    Impl()
        : isGridVisible(false)
        , areNormalsVisible(false)
        , selectBuffer(NULL)
        , aspectRatio(1.0)
        , viewType(Front)
        , drawingMode(DrawingMode::Points)
        , renderer(NULL)
        , selectionRenderer(NULL)
        , editVertexRenderer(NULL)
        , cursorShape(None)
        , zoomFactor(1.0)
        , textureId(0)
        , pen(QPen(QColor(Qt::white)))
        , brush(QBrush(Qt::transparent))
        , flags(0)
    {
        // Type of renderer used for displaying objects on the screen
        renderer = RendererFactory::getRenderer(drawingMode);

        // Type of renderer used for user object picking
        selectionRenderer = RendererFactory::getRenderer(DrawingMode::Points);

        // Type of renderer used for vertex edition mode
        editVertexRenderer = RendererFactory::getRenderer(DrawingMode::Points);

        selectBuffer = new GLuint[SELECT_BUFFER_SIZE];

        auto camera = std::make_shared<Camera>();
        camera->transform().translate(Vector3(0, 0, -6));
        cameraList[Front] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().rotate(
            Eigen::AngleAxisf(float(M_PI), Vector3(0, 1, 0)));
        camera->transform().translate(Vector3(0, 0, 6));
        cameraList[Back] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().rotate(
            Eigen::AngleAxisf(float(M_PI_2), Vector3(1, 0, 0)));
        camera->transform().translate(Vector3(0, 6, 0));
        cameraList[Top] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().rotate(
            Eigen::AngleAxisf(float(-M_PI_2), Vector3(1, 0, 0)));
        camera->transform().translate(Vector3(0, -6, 0));
        cameraList[Bottom] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().rotate(
            Eigen::AngleAxisf(float(-M_PI_2), Vector3(0, 1, 0)));
        camera->transform().translate(Vector3(-6, 0, 0));
        cameraList[Left] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().rotate(
            Eigen::AngleAxisf(float(M_PI_2), Vector3(0, 1, 0)));
        camera->transform().translate(Vector3(6, 0, 0));
        cameraList[Right] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().translate(Vector3(-6, 6, 0));
        camera->transform().rotate(
            Eigen::AngleAxisf(float(-M_PI_2), Vector3(1, 1, 0).normalized()));
        cameraList[Perspective] = camera;

        camera = std::make_shared<Camera>();
        camera->transform().translate(Vector3(-6, 6, 0));
        camera->transform().rotate(
            Eigen::AngleAxisf(float(-M_PI_2), Vector3(1, 1, 0).normalized()));
        cameraList[CameraView] = camera;
    }

    ~Impl()
    {
        if (selectBuffer)
            delete[] selectBuffer;

        delete renderer;
        renderer = NULL;

        delete selectionRenderer;
        selectionRenderer = NULL;
    }

    void setupShaderCommonParams();
};

void GlCanvas::Impl::setupShaderCommonParams() {}

GlCanvas::GlCanvas(DocumentView* _parent)
    : QOpenGLWidget(_parent)
    , _d(new Impl)
{
}

GlCanvas::~GlCanvas() {}

GlCanvas::PerspectiveType GlCanvas::getPerspectiveView()
{
    return _d->viewType;
}

GlCanvas::CursorShapeType GlCanvas::getCursorShape()
{
    return _d->cursorShape;
}

void GlCanvas::setCursorPosition(Point3 p) { _d->cursorPosition = p; }

Point3 GlCanvas::getCursorPosition() { return _d->cursorPosition; }

void GlCanvas::setCursorOrientation(Point3 n) { _d->cursorOrientation = n; }

Point3 GlCanvas::getCursorOrientation() { return _d->cursorOrientation; }

IRenderer* GlCanvas::renderer() const { return _d->renderer; }

void GlCanvas::setPerspectiveView(PerspectiveType type)
{
    _d->viewType = type;
}

void GlCanvas::setGridVisible(bool value)
{
    _d->isGridVisible = value;
    update();
}

bool GlCanvas::isGridVisible() { return _d->isGridVisible; }

bool GlCanvas::areNormalsVisible() { return _d->areNormalsVisible; }

void GlCanvas::setNormalsVisible(bool visible)
{
    _d->areNormalsVisible = visible;
}

void GlCanvas::setDrawingMode(DrawingMode mode)
{
    _d->drawingMode = mode;
    delete _d->renderer;
    _d->renderer = RendererFactory::getRenderer(_d->drawingMode);
}

void GlCanvas::initializeGL()
{
    // Set up the rendering context, define display lists etc.:
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL |
                                PlastilinaSubsystem::OPENCL |
                                PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
    if (!PlastilinaEngine::initialize(flags))
    {
        return;
    }
    // glClearColor( 0.4, 0.4, 0.4, 1.0 );
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0f);
    enable(GL_DEPTH_TEST);
    enable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
        qDebug("Depth buffer not enabled.");
    else
    {
        qDebug("Depth buffer enabled. Precision: %d",
               format().depthBufferSize());
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    std::string str((const char*)glGetString(GL_VERSION));
    qDebug() << "GLVersion: " << str.c_str();
}

void GlCanvas::resizeGL(int w, int h)
{
    if (h > 0)
        _d->aspectRatio = GLdouble(w) / GLdouble(h);

    // setup viewport, projection etc. for OpenGL:
    glViewport(0, 0, (GLint)w, (GLint)h);
    glGetIntegerv(GL_VIEWPORT, _d->viewport);

    // Now update the cameras to have the same viewport and projection
    // information
    foreach (std::shared_ptr<Camera> c, _d->cameraList)
    {
        c->setViewport(0, 0, w, h);
        c->setOrthoMatrix(
            -DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
            DEFAULT_HEIGHT / 2 * _d->zoomFactor * _d->aspectRatio,
            -DEFAULT_HEIGHT / 2 * _d->zoomFactor,
            DEFAULT_HEIGHT / 2 * _d->zoomFactor, -1000.0, 1000.0);
    }
}

void GlCanvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);

    IDocument::shared_ptr doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc)
    {
        auto                   scene = doc->scene().lock();
        CameraNode::shared_ptr cam   = doc->getCamera();
        if (cam)
        {
            std::shared_ptr<Camera> camera =
                _d->cameraList.contains(_d->viewType)
                    ? _d->cameraList[_d->viewType]
                    : NULL;
            cam->setCamera(camera);
        }
        else
        {
            qDebug() << "Failed to get camera!";
        }

        if (_d->isGridVisible)
            drawGrid();

        glLineWidth(1);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        enable(GL_BLEND);
        enable(GL_LINE_SMOOTH);

        drawScene(scene);
    }

    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();
    if (cmd)
    {
        cmd->paintGL(this);
    }

    glFlush();
}

void GlCanvas::drawScene(Scene::shared_ptr scene)
{
    try
    {
        scene->render();
    }
    catch (core::GlException& e)
    {
        std::cerr << "GLException: " << e.what() << std::endl
                  << e.error() << ": " << e.errorString() << std::endl;
    }
}

void GlCanvas::drawSceneNode(Scene::shared_ptr&     scene,
                             SceneNode::shared_ptr& node)
{
}

void GlCanvas::drawBoundingBox(const ISurface* mesh)
{
    using namespace geometry;

    //    AABB bb = mesh->boundingBox();
    //    float minX = bb.min().x() - 0.1;
    //    float minY = bb.min().y() - 0.1;
    //    float minZ = bb.min().z() - 0.1;
    //    float maxX = bb.max().x() + 0.1;
    //    float maxY = bb.max().y() + 0.1;
    //    float maxZ = bb.max().z() + 0.1;
}

void GlCanvas::drawGrid() {}

void GlCanvas::drawCursor()
{
    if (_d->cursorShape == None)
        return;

    switch (_d->cursorShape)
    {
    case None:
    case Cross:
    case Circle:
        break;
    case Image:
    {
    }
    break;
    }
}

void GlCanvas::drawOrientationAxis() {}

void GlCanvas::mouseMoveEvent(QMouseEvent* e)
{
    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();

    bool needUpdate = false;
    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
        // this->getViewCamera()->
    }
    else
    {
        if (cmd)
        {
            cmd->mouseMoveEvent(e);
            needUpdate = true;
        }
        if (_d->cursorShape != None)
        {
            //_cursorPosition.setPoint(e->x(), e->y(), 0);
            _d->cursorPosition.x() = e->x();
            _d->cursorPosition.y() = e->y();
            _d->cursorPosition.z() = 0;
            needUpdate             = true;
        }
    }

    if (needUpdate)
    {
        update();
    }
}

void GlCanvas::mousePressEvent(QMouseEvent* e)
{
    // qDebug("MousePress");
    ICommand* cmd        = g_pApp->getMainWindow()->getSelectedCommand();
    bool      needUpdate = false;

    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
    }
    else
    {
        if (cmd)
        {
            cmd->mousePressEvent(e);
            needUpdate = true;
        }
        _d->cursorPosition.x() = e->x();
        _d->cursorPosition.y() = e->y();
        if (_d->cursorShape != None)
        {
            _d->cursorPosition.x() = e->x();
            _d->cursorPosition.y() = e->y();
            needUpdate             = true;
        }
    }
    if (needUpdate)
    {
        update();
    }
}

void GlCanvas::mouseReleaseEvent(QMouseEvent* e)
{
    // qDebug("MouseRelease");
    ICommand* cmd = g_pApp->getMainWindow()->getSelectedCommand();

    if (e->modifiers() & Qt::AltModifier)
    {
        qDebug() << "Alt pressed";
    }
    else
    {
        if (cmd)
        {
            cmd->mouseReleaseEvent(e);
            update();
        }
    }
}

void GlCanvas::wheelEvent(QWheelEvent* e)
{
    int numDegrees = e->delta() / 8;
    int numSteps   = numDegrees / 15;

    _d->zoomFactor += numSteps * 0.01;

    resizeGL(width(), height());
    update();
}

ObjectContainer GlCanvas::getSelectedObjects(GLint x, GLint y)
{
    ObjectContainer l;
    if (_d->renderer == NULL)
        return l;

    unsigned int objId = 1;
    GLuint       d     = 0;
    ISurface*    mesh;
    auto doc = ((DocumentView*)parentWidget())->getDocument()->scene().lock();
    if (!doc)
    {
        return l;
    }

    Iterator<SceneNode> it = doc->iterator();
    while (it.hasNext())
    {
        auto                    n = it.next();
        SurfaceNode::shared_ptr s;
        if (n->nodeType() == NT_Surface)
        {
            s    = std::static_pointer_cast<SurfaceNode>(n);
            mesh = s->surface();
            // Point3 p = s->localToWorld(Point3(0,0,0));

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_picking.renderObject(n);
            glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_RGBA,
                         GL_UNSIGNED_BYTE, (GLubyte*)&d);

            if (d == objId)
                l.push_back(mesh);

            objId++;
        }
    }

    return l;
}

PointIndexList GlCanvas::getSelectedVertices(GLint x, GLint y, GLint width,
                                             GLint height)
{
    PointIndexList l;
    if (_d->renderer == NULL || width == 0 || height == 0)
        return l;

    int halfWidth  = width / 2;
    int halfHeight = height / 2;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    unsigned int objId = 1;
    GLuint*      d     = new GLuint[width * height];
    ISurface*    mesh;

    auto doc = ((DocumentView*)parentWidget())->getDocument()->scene().lock();
    if (!doc)
    {
        return l;
    }
    Iterator<SceneNode> it = doc->iterator();
    while (it.hasNext())
    {
        memset(d, 0, width * height * sizeof(GLuint));

        auto                    n = it.next();
        SurfaceNode::shared_ptr s;
        if (n->nodeType() == NT_Surface)
        {
            s    = std::static_pointer_cast<SurfaceNode>(n);
            mesh = s->surface();
            // Point3 p = s->localToWorld(Point3(0,0,0));

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            g_pickingVertices.renderObject(n);
            glReadPixels(x - halfWidth, _d->viewport[3] - (y + halfHeight),
                         width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                         (GLubyte*)d);

            int faceIndex = 0;
            for (int j = 0; j < width * height; ++j)
            {
                if (d[j] == 0)
                    continue;

                faceIndex = d[j] - 1;
                auto f    = mesh->face(faceIndex);

                // auto vtxIt = f->constVertexIterator();
                // while(vtxIt.hasNext()) {
                // const Vertex& v = vtxIt.next();
                //}
            }

            objId++;
        }
    }

    delete[] d;
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return l;
}

std::shared_ptr<Camera> GlCanvas::getViewCamera()
{
    if (!_d->cameraList.contains(_d->viewType))
    {
        // qDebug("camera %d does not exists. Returning perspective camera.",
        // _d->viewType);
        return _d->cameraList[Perspective];
    }
    return _d->cameraList[_d->viewType];
}

const std::shared_ptr<Camera> GlCanvas::getViewCamera() const
{
    if (!_d->cameraList.contains(_d->viewType))
    {
        // qDebug("camera %d does not exists. Returning perspective camera.",
        // _d->viewType);
        return _d->cameraList[Perspective];
    }
    return _d->cameraList[_d->viewType];
}

void GlCanvas::setViewCamera(PerspectiveType          viewType,
                             std::shared_ptr<Camera>& camera)
{
    if (!_d->cameraList.contains(viewType))
    {
        qDebug("camera %d does not exists. Seeting as a new camera.",
               _d->viewType);
    }
    _d->cameraList[viewType] = camera;
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
        //// Delete any texture loaded before.
        // deleteTexture(_d->textureId);
        //_d->textureId = 0;

        //_d->cursorImage = image;
        //_d->textureId = bindTexture(_d->cursorImage);
        // if (_d->textureId == 0)
        //{
        //    qDebug("texture id is not valid");
        //}
    }
}

/**
 * Returns a copy of the image used as cursor.
 */
QImage GlCanvas::getCursorImage() { return _d->cursorImage; }

void GlCanvas::screenToWorld(double x, double y, double z, double* wx,
                             double* wy, double* wz) const
{
    Point3 p(x, y, z);
    Point3 o = screenToWorld(p);
    if (wx)
        *wx = o.x();
    if (wy)
        *wy = o.y();
    if (wz)
        *wz = o.z();
}

void GlCanvas::screenToWorld(const Point3& winCoords, Point3& world) const
{
    world = screenToWorld(winCoords);
}

Point3 GlCanvas::screenToWorld(const Point3& p) const
{
    auto   cam = getViewCamera();
    Camera c(*cam);
    c.setViewport(_d->viewport[0], _d->viewport[1], _d->viewport[2],
                  _d->viewport[3]);
    return c.screenToWorld(p);
}

void GlCanvas::worldToScreen(double wx, double wy, double wz, double* x,
                             double* y, double* z) const
{
    Point3 p(wx, wy, wz);
    Point3 o = worldToScreen(p);
    if (x)
        *x = o.x();
    if (y)
        *y = o.y();
    if (z)
        *z = o.z();
}

void GlCanvas::worldToScreen(const Point3& world, Point3& winCoords) const
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
void GlCanvas::setPen(const QPen& pen) { _d->pen = pen; }

/**
 *
 */
const QPen& GlCanvas::pen() const { return _d->pen; }

/**
 *
 */
void GlCanvas::setBrush(const QBrush& brush) { _d->brush = brush; }

/**
 *
 */
const QBrush& GlCanvas::brush() const { return _d->brush; }

Color GlCanvas::color(const Point3& p)
{
    GLint x = p[0];
    GLint y = p[1];
    return color(x, y);
}

Color GlCanvas::color(int x, int y)
{
    return Color::fromUintRGBA(colorUint(x, y));
}

uint32_t GlCanvas::colorUint(const Point3& p)
{
    GLint x = p[0];
    GLint y = p[1];
    return colorUint(x, y);
}

uint32_t GlCanvas::colorUint(int x, int y)
{
    GLuint d;

    glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                 (GLubyte*)&d);

    return d;
}

float GlCanvas::depth(const Point3& p)
{
    GLint x = p[0];
    GLint y = p[1];
    return depth(x, y);
}

float GlCanvas::depth(int x, int y)
{
    float d;

    glReadPixels(x, _d->viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT,
                 &d);
    return d;
}

void GlCanvas::begin(GLenum mode) { _d->flags |= GLCANVAS_BEGIN; }

void GlCanvas::end() { _d->flags &= ~GLCANVAS_BEGIN; }

void GlCanvas::drawLine(const Point3& p1, const Point3& p2)
{
    double c[4];
    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0)
    {
        //        glColor4dv(c);
        //        if (!_d->flags & GLCANVAS_BEGIN) glBegin(GL_LINES);
        //        glVertex3fv(p1.data());
        //        glVertex3fv(p2.data());
        //        if (!_d->flags & GLCANVAS_BEGIN) glEnd();
    }
}

void GlCanvas::drawLine2D(const Point3& p1, const Point3& p2)
{
    double c[4];
    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0)
    {
        //        Point3 tmp1, tmp2;
        //        screenToWorld(p1, tmp1);
        //        screenToWorld(p2, tmp2);
        //        glColor4dv(c);
        //        if (!_d->flags & GLCANVAS_BEGIN) glBegin(GL_LINES);
        //        glVertex3fv(tmp1.data());
        //        glVertex3fv(tmp2.data());
        //        if (!_d->flags & GLCANVAS_BEGIN) glEnd();
    }
}

void GlCanvas::drawRect(const Point3& p1, const Point3& p2, int mode)
{
    switch (mode)
    {
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
    if (c[3] > 0)
    {
        //        glColor4dv(c);
        //        glBegin(GL_QUADS);
        //        glVertex3fv(p1.data());
        //        glVertex3fv(p2.data());
        //        glVertex3fv(p3.data());
        //        glVertex3fv(p4.data());
        //        glEnd();
    }

    _d->pen.color().getRgbF(&c[0], &c[1], &c[2], &c[3]);
    if (c[3] > 0)
    {
        //        glColor4dv(c);
        //        glBegin(GL_LINE_LOOP);
        //        glVertex3fv(p1.data());
        //        glVertex3fv(p2.data());
        //        glVertex3fv(p3.data());
        //        glVertex3fv(p4.data());
        //        glVertex3fv(p1.data());
        //        glEnd();
    }
}

/**
 *
 */
void GlCanvas::drawEllipse(const Point3& center, float axis1, float axis2,
                           float innerAxis1, float innerAxis2)
{
    drawEllipseWinCoord(center, 0, 360, axis1, axis2, innerAxis1, innerAxis2);
}

void GlCanvas::drawArc(const Point3& center, float startAngle, float endAngle,
                       float axis1, float axis2, float innerAxis1,
                       float innerAxis2)
{
    drawEllipseWinCoord(center, startAngle, endAngle, axis1, axis2,
                        innerAxis1, innerAxis2);
}

// Helper function to compute the next value in an ellipse
static void calculateEllipse(double x, double y, double a, double b,
                             double startAngle, double endAngle, double angle,
                             int steps, std::vector<Point3>& points)
{
    if (steps == 0)
        steps = 36;

    points.clear();
    points.reserve(steps + 1);

    double deltaAngle = endAngle - startAngle;
    // Angle is given by Degree Value
    double radFactor = M_PI / 180;      // factor to convert to radians
    double beta    = angle * radFactor; // converts Degree Value into Radians
    double sinbeta = sin(beta);
    double cosbeta = cos(beta);
    double angleStep = deltaAngle / steps * radFactor;
    double alpha     = startAngle * radFactor;
    for (int j = 0; j <= steps; ++j)
    {
        double sinalpha = sin(alpha);
        double cosalpha = cos(alpha);

        double X = x + (a * cosalpha * cosbeta - b * sinalpha * sinbeta);
        double Y = y + (a * cosalpha * sinbeta + b * sinalpha * cosbeta);

        points.push_back(Point3(X, Y, 0));

        alpha += angleStep;
    }
}

void GlCanvas::drawEllipseWinCoord(const Point3& center, float startAngle,
                                   float endAngle, float axis1, float axis2,
                                   float innerAxis1, float innerAxis2)
{
    const int           NUM_POINTS = 36;
    std::vector<Point3> points;
    std::vector<Point3> innerPoints;
    bool                drawInnerEllipse = innerAxis1 != 0 && innerAxis2 != 0;

    calculateEllipse(center.x(), center.y(), axis1, axis2, 0, endAngle,
                     startAngle, NUM_POINTS, points);
    if (drawInnerEllipse)
    {
        calculateEllipse(center.x(), center.y(), innerAxis1, innerAxis2, 0,
                         endAngle, startAngle, NUM_POINTS, innerPoints);
    }
    int npoints = points.size();
    for (int i = 0; i < npoints; ++i)
    {
        screenToWorld(points[i], points[i]);
        if (drawInnerEllipse)
        {
            screenToWorld(innerPoints[i], innerPoints[i]);
        }
    }

    double color[4];
    _d->brush.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    //    glColor4dv(color);
    //    glBegin(GL_TRIANGLES);
    //    if (drawInnerEllipse) {
    //        for (int i = 0; i < npoints - 1; ++i) {
    //            glVertex3fv(points[i].data());
    //            glVertex3fv(innerPoints[i].data());
    //            glVertex3fv(points[i+1].data());
    //            glVertex3fv(innerPoints[i].data());
    //            glVertex3fv(points[i+1].data());
    //            glVertex3fv(innerPoints[i+1].data());
    //        }
    //    }
    //    else {
    //        Point3 c;
    //        screenToWorld(center, c);
    //        for (int i = 0; i < npoints - 1; ++i) {
    //            glVertex3fv(points[i].data());
    //            glVertex3fv(c.data());
    //            glVertex3fv(points[i+1].data());
    //        }
    //    }
    //    glEnd();

    //    _d->pen.color().getRgbF(&color[0], &color[1], &color[2], &color[3]);
    //    glColor4dv(color);
    //    // draw main ellipse
    //    glBegin(GL_LINE_STRIP);
    //    for (int i = 0; i < npoints; ++i) {
    //        glVertex3fv(points[i].data());
    //    }
    //    glEnd();
    //
    //    // draw inner ellipse
    //    if (drawInnerEllipse) {
    //        glBegin(GL_LINE_STRIP);
    //        for (int i = 0; i < npoints; ++i) {
    //            glVertex3fv(innerPoints[i].data());
    //        }
    //        glEnd();
    //    }
}
