/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
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
#define NOT_IMPLEMENTED
#define EIGEN_INITIALIZE_MATRICES_BY_ZERO
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtOpenGL>
#include <QDebug>
#include <iostream>
#include <QColor>
#include <QVector>

#include <OpenGL.h>
#include <CGLTypes.h>

#include "ray.h"
#include "Plane.h"
#include "Subdivision.h"
#include "HEdge.h"
#include "Camera.h"


using namespace std;

class QSculptEngineTest : public QObject
{
    Q_OBJECT

public:
    QSculptEngineTest();

private Q_SLOTS:
    void testCase1();

    void testSubdivisionFaceIterator();

    void testCamera();

private:
    bool createGLContext();
    bool destroyGLContext();

    CGLContextObj   _contextObj;
    void*           _buffer;
};

QSculptEngineTest::QSculptEngineTest()
{
    _buffer = NULL;
    _contextObj = NULL;
}

bool QSculptEngineTest::createGLContext()
{
    bool res = true;

    // Pixel format attrubutes for an offscreen renderer
    CGLPixelFormatAttribute attribs[] =
    {
        kCGLPFAOffScreen,
        kCGLPFAColorSize,
        (CGLPixelFormatAttribute)32,
        (CGLPixelFormatAttribute)NULL
    };
    CGLPixelFormatObj pixelFormatObj = NULL;
    int numPixelFormats;
    if (CGLChoosePixelFormat(attribs, &pixelFormatObj, &numPixelFormats) != kCGLNoError)
    {
        res = false;
        goto exit;
    }

    if (CGLCreateContext(pixelFormatObj, NULL, &_contextObj) != kCGLNoError)
    {
        res = false;
        goto exit;
    }


    if (CGLSetCurrentContext(_contextObj) != kCGLNoError)
    {
        res = false;
        goto exit;
    }

    _buffer = (void *) malloc(640 * 480 * 32 / 8);	//5
    if (!_buffer) {
        res = false;
        goto exit;
    }

    if (CGLSetOffScreen(_contextObj, 640, 480, 640 * 4, _buffer) != kCGLNoError)
    {
        res = false;
        goto exit;
    }
    //***** Perform offscreen drawing
    exit:
    if (pixelFormatObj) CGLDestroyPixelFormat (pixelFormatObj);
    if (!res) {
        if (_contextObj) CGLDestroyContext(_contextObj);
        if (_buffer) free(_buffer);
    }
    return res;
}

bool QSculptEngineTest::destroyGLContext()
{
    CGLSetCurrentContext (NULL);
    if (_contextObj) {
        CGLClearDrawable(_contextObj);
        CGLDestroyContext(_contextObj);
    }
    return true;
}

void QSculptEngineTest::testCase1()
{
    geometry::Ray ray(Point3(0, 0, -1), Vector3(0, 0, 1));
    geometry::Plane plane;

    cout << ray.intersect(plane) << endl;

    QVERIFY2(true, "Failure");
}

void QSculptEngineTest::testSubdivisionFaceIterator()
{
    Subdivision surf;

    double hw = 1.0;
    double hh = 1.0;
    double hd = 1.0;

    QVector<int> vertexId;
    QVector<int> faceId;

    vertexId.append(surf.addVertex(new Vertex(Point3(-hw, hh,-hd), Vector3(-hw, hh,-hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3( hw, hh,-hd), Vector3( hw, hh,-hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3( hw,-hh,-hd), Vector3( hw,-hh,-hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3(-hw,-hh,-hd), Vector3(-hw,-hh,-hd))));

    vertexId.append(surf.addVertex(new Vertex(Point3(-hw, hh, hd), Vector3(-hw, hh, hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3( hw, hh, hd), Vector3( hw, hh, hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3( hw,-hh, hd), Vector3( hw,-hh, hd))));
    vertexId.append(surf.addVertex(new Vertex(Point3(-hw,-hh, hd), Vector3(-hw,-hh, hd))));

    QVector<int> indexList(4);
    indexList[0] = vertexId[0];
    indexList[1] = vertexId[1];
    indexList[2] = vertexId[2];
    indexList[3] = vertexId[3];
    faceId.append(surf.addFace( indexList ));
//    indexList[0] = 4;
//    indexList[1] = 7;
//    indexList[2] = 6;
//    indexList[3] = 5;
//    faceId.append(surf.addFace( indexList ));
//    indexList[0] = 0;
//    indexList[1] = 3;
//    indexList[2] = 7;
//    indexList[3] = 4;
//    faceId.append(surf.addFace( indexList ));
//    indexList[0] = 5;
//    indexList[1] = 6;
//    indexList[2] = 2;
//    indexList[3] = 1;
//    faceId.append(surf.addFace( indexList ));
//    indexList[0] = 0;
//    indexList[1] = 4;
//    indexList[2] = 5;
//    indexList[3] = 1;
//    faceId.append(surf.addFace( indexList ));
//    indexList[0] = 7;
//    indexList[1] = 3;
//    indexList[2] = 2;
//    indexList[3] = 6;
//    faceId.append(surf.addFace( indexList ));

    qDebug() << "Forward iteration:";
    Iterator<Face> faceIt = surf.faceIterator();
    while(faceIt.hasNext()) {
        qDebug() << faceIt.next().iid();
    }

    qDebug() << "Forward iteration:";
    faceIt = surf.faceIterator();
    while(faceIt.hasNext()) {
        qDebug() << faceIt.next().iid();
    }

    qDebug() << "Vertex iterator:";
    int i = 0;
    Iterator<Vertex> vtxIt = surf.vertexIterator();
    while(vtxIt.hasNext()) {
        ++i;
        qDebug() << i ;
        qDebug() << " vtx: " << vtxIt.next().iid();
    }

    qDebug() << "Edge iterator:";
    faceIt.seek(0);
    Iterator<Edge> edgeIt = faceIt.next().edgeIterator();
    Point3 p; int c = 0;
    while(edgeIt.hasNext()){
        Edge& e = edgeIt.next();
        qDebug() << "edge: " << e.iid() << "v: " << e.pair()->head()->iid() << " - " << e.head()->iid();
        std::cout << (e.tail()->position() + e.head()->position() ) * 0.25f << std::endl;
        p += e.head()->position();
        c++;
        //break;
    }
    std::cout << "Average: " << p / c << std::endl;
}

void QSculptEngineTest::testCamera()
{
    Camera camera;

    int x = 0;
    int y = 0;
    int w = 640;
    int h = 480;

    float l = -1;
    float r = 1;
    float b = -1;
    float t = 1;
    float n = -1;
    float f = 1;

    Point3 eye(-1, 0, 0);
    Point3 target(0, 0, 0);
    Vector3 up(0, 1, 0);

    camera.setViewport(x, y, w, h);
    camera.setOrthoMatrix(l, r, b, t, n, f);
    camera.setPosition(eye);
    camera.setTargetPoint(target);
    camera.setOrientationVector(up);

    qDebug() << camera.toString();

    cout << camera.viewport() << endl << endl;
    cout << camera.modelView() << endl << endl;
    cout << camera.projection() << endl << endl;
    Point3 p;
    qDebug() << toString(p) << toString(camera.worldToEye(p)) << endl;
    p = camera.worldToEye(p);
    qDebug() << toString(p) << toString(camera.eyeToWorld(p)) << endl;

    p = Point3(0.5, 0.5, 1);
    qDebug() << toString(p) << toString(camera.worldToEye(p)) << endl;
    p = camera.worldToEye(p);
    qDebug() << toString(p) << toString(camera.eyeToWorld(p)) << endl;

    if (!createGLContext()) {
        qDebug() << "Failed to create context" ;
        return;
    }

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(l, r, b, t, n, f);
    glLoadMatrixf(camera.projection().data());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(camera.modelView().data());
    //gluLookAt(eye.x(), eye.y(), eye.z(), target.x(), target.y(), target.z(), up.x(), up.y(), up.z());
    //gluLookAt( 1, 0, 0, 0, 0, 0, 0, 1, 0);

    int viewport[4];
    double modelMatrix[16], projMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    Eigen::Matrix4d m(modelMatrix);
    cout << m << endl;
    m = Eigen::Matrix4d(projMatrix);
    cout << m << endl;

    GLdouble dx, dy, dz;
    GLdouble wx = 0, wy = 0, wz = 0;

    p = Point3(0, 0, 0);
    dx = GLdouble(p.x());
    dy = GLdouble(p.y());
    dz = GLdouble(p.z());
    gluProject( dx, dy, dz, modelMatrix, projMatrix, viewport,
                  &wx, &wy, &wz);
    qDebug() << "(" << dx << ", " << dy << ", " << dz << ") (" << wx << ", " << wy << ", " << wz << ")";
    p = Point3(-1, -1, -1);
    dx = GLdouble(p.x());
    dy = GLdouble(p.y());
    dz = GLdouble(p.z());
    gluProject( dx, dy, dz, modelMatrix, projMatrix, viewport,
                  &wx, &wy, &wz);
    qDebug() << "(" << dx << ", " << dy << ", " << dz << ") (" << wx << ", " << wy << ", " << wz << ")";

    p = Point3(0.5, 0.5, 1);
    dx = GLdouble(p.x());
    dy = GLdouble(p.y());
    dz = GLdouble(p.z());
    gluProject( dx, dy, dz, modelMatrix, projMatrix, viewport,
                  &wx, &wy, &wz);
    qDebug() << "(" << dx << ", " << dy << ", " << dz << ") (" << wx << ", " << wy << ", " << wz << ")";

    destroyGLContext();
}

QTEST_APPLESS_MAIN(QSculptEngineTest);

#include "tst_qsculptenginetest.moc"
