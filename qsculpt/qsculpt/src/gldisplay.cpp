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
#include "gldisplay.h"
#include <QtOpenGL>
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include "sphere.h"
#include "box.h"
#include "idocument.h"
#include "qsculptwindow.h"
#include "qsculptapp.h"
#include "icommand.h"
#include "documentview.h"
#include "camera.h"

#define SELECT_BUFFER_SIZE 512
#define DEFAULT_HEIGHT 5.0f

GlDisplay::GlDisplay(DocumentView* parent)
 : QGLWidget(parent),
    m_isGridVisible(false),
    m_areNormalsVisible(false),
    m_selectBuffer(NULL),
    m_aspectRatio(1.0),
    m_viewType(Front),
    m_drawingMode(Points),
    m_cursorShape(None),
    m_zoomFactor(1.0)
{
    m_selectBuffer = new GLuint[SELECT_BUFFER_SIZE];
    
    Camera* camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector(Point3D( 0, 1, 0) );
    camera->setPosition( Point3D( 0, 0, 1));
    m_cameraList[Front] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 1, 0) );
    camera->setPosition( Point3D( 0, 0, -1));
    m_cameraList[Back] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 0, -1) );
    camera->setPosition( Point3D( 0, 1, 0) );
    m_cameraList[Top] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 0, 1) );
    camera->setPosition( Point3D( 0, -1, 0) );
    m_cameraList[Bottom] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 1, 0) );
    camera->setPosition( Point3D(-1, 0, 0) );
    m_cameraList[Left] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 1, 0) );
    camera->setPosition( Point3D( 1, 0, 0) );
    m_cameraList[Right] = camera;
    
    camera = new Camera();
    camera->setTargetPoint( Point3D( 0, 0, 0) );
    camera->setOrientationVector( Point3D( 0, 0, 1) );
    camera->setPosition( Point3D( 0.75, 0.75, 0.75) );
    m_cameraList[Perspective] = camera;
}


GlDisplay::~GlDisplay()
{
    if (m_selectBuffer)
        delete [] m_selectBuffer;
    
    QMutableMapIterator<int, Camera*> it(m_cameraList);
    while(it.hasNext())
    {
        it.next();
        delete it.value();
        it.remove();
    }
}

void GlDisplay::setGridVisible(bool value)
{
    m_isGridVisible = value;
    updateGL();
}

bool GlDisplay::isGridVisible()
{
    return m_isGridVisible;
}

bool GlDisplay::areNormalsVisible()
{
    return m_areNormalsVisible;
}

void GlDisplay::setNormalsVisible(bool visible)
{
    m_areNormalsVisible = visible;
}

void GlDisplay::initializeGL()
{
    // Set up the rendering context, define display lists etc.:
    //float lightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
    //float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    //float lightPosition[] = { 0.0f, 0.0f, 1.0f, 1.0f};
    
    glClearColor( 0.4, 0.4, 0.4, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glShadeModel(GL_SMOOTH);
    //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
    glSelectBuffer(SELECT_BUFFER_SIZE, m_selectBuffer);

    if (glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
        qDebug("Depth buffer not enabled.");
    else
    {
        qDebug("Depth buffer enabled. Precision: %d", format().depthBufferSize());
    }
}

void GlDisplay::resizeGL( int w, int h )
{
    if ( h > 0 )
        m_aspectRatio = GLdouble( w ) / GLdouble( h );

    // setup viewport, projection etc.:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -DEFAULT_HEIGHT / 2 * m_zoomFactor * m_aspectRatio,
              DEFAULT_HEIGHT / 2 * m_zoomFactor * m_aspectRatio,
             -DEFAULT_HEIGHT / 2 * m_zoomFactor,
              DEFAULT_HEIGHT / 2 * m_zoomFactor,
             -1000.0,
              1000.0 );
    /*
    glOrtho( (w % 2) == 0 ? -w/2 * m_zoomFactor: -(w+1)/2* m_zoomFactor,
             (w % 2) == 0 ? w/2 * m_zoomFactor: (w+1)/2 * m_zoomFactor,
             (h % 2) == 0 ? -h/2 * m_zoomFactor: -(h+1)/2* m_zoomFactor,
             (h % 2) == 0 ? h/2 * m_zoomFactor: (h+1)/2 * m_zoomFactor,
             -100.0,
             100.0 );
     */
    //glFrustum(-5.0 * m_aspectRatio, 5.0 * m_aspectRatio, -5.0, 5.0, 0.5, 500.0);
    glMatrixMode( GL_MODELVIEW );
    
    glGetIntegerv(GL_VIEWPORT, m_viewport);
}

void GlDisplay::paintGL()
{
    //Point3D point( 3, 2, 4 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    
    Camera* camera = m_cameraList.contains(m_viewType) ? m_cameraList[m_viewType] : NULL;
    if (camera)
    {
        gluLookAt( camera->getPosition().getX(), camera->getPosition().getY(),
                camera->getPosition().getZ(), camera->getTargetPoint().getX(),
                camera->getTargetPoint().getY(), camera->getTargetPoint().getZ(),
                camera->getOrientationVector().getX(), camera->getOrientationVector().getY(),
                camera->getOrientationVector().getZ());
        //qDebug(qPrintable(camera->toString()));
    }
    
    //glTranslatef(-100, -100, 0);
    glDisable(GL_LIGHTING);
    
    if (m_isGridVisible)
        drawGrid();

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
    
    drawCursor();

    switch(m_drawingMode)
    {
        case Smooth:
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH);
            break;
        case Flat:
            glEnable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            break;
        case Wireframe:
        case Texture:
        case Points:
            break;
    }
    drawObjects();
    
    //drawOrientationAxis();
}

void GlDisplay::drawObjects()
{
    IDocument* doc= ((DocumentView*)parentWidget())->getDocument();
    int count = doc->getObjectsCount();
    for ( int i = 0; i < count; i++ )
    {
        glLoadName(i+1);
        doc->getObject(i)->setDrawingMode( m_drawingMode );
        doc->getObject(i)->draw();
    }
}

void GlDisplay::drawGrid()
{
    const double GRID_PLANE_Z = 0.0;
    glLineWidth(1.0);
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
}

void GlDisplay::drawCursor()
{
    m_cursorShape = Cross;
    if (m_cursorShape == None)
        return;

    m_cursorPosition.setX(0.0);
    m_cursorPosition.setY(0.0);
    m_cursorPosition.setZ(2.0);

    switch(m_cursorShape)
    {
    case Cross:
        glBegin(GL_LINES);
        glVertex3f( m_cursorPosition.getX() - 0.1, m_cursorPosition.getY(),m_cursorPosition.getZ());
        glVertex3f( m_cursorPosition.getX() + 0.1, m_cursorPosition.getY(),m_cursorPosition.getZ());
        
        glVertex3f( m_cursorPosition.getX(), m_cursorPosition.getY() - 0.1, m_cursorPosition.getZ());
        glVertex3f( m_cursorPosition.getX(), m_cursorPosition.getY() + 0.1, m_cursorPosition.getZ());
        glEnd();
        break;
        
    case Circle:

        break;
        
    default:
        break;
    }
}

void GlDisplay::drawOrientationAxis()
{
    bool lightEnabled = glIsEnabled(GL_LIGHTING);
    
    if (lightEnabled)
        glDisable(GL_LIGHTING);
    
    // setup viewport, projection etc.:
    glViewport( 0, 0, 100, 100 );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( -1.5 * m_aspectRatio, 1.5 * m_aspectRatio, 1.5, -1.5, 2.0, -2.0 );
    
    
    glMatrixMode( GL_MODELVIEW );
    
    Camera* camera = m_cameraList.contains(m_viewType) ? m_cameraList[m_viewType] : NULL;
    if (camera)
    {
        gluLookAt( camera->getPosition().getX(), camera->getPosition().getY(),
                   camera->getPosition().getZ(), camera->getTargetPoint().getX(),
                   camera->getTargetPoint().getY(), camera->getTargetPoint().getZ(),
                   camera->getOrientationVector().getX(), camera->getOrientationVector().getY(),
                   camera->getOrientationVector().getZ());
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
    
    glViewport( m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3] );
    
    if (lightEnabled)
        glEnable(GL_LIGHTING);
}

void GlDisplay::mouseMoveEvent ( QMouseEvent * e )
{
    ICommand* cmd = SPAPP->getMainWindow()->getSelectedCommand();
    
    if (cmd)
    {
        cmd->mouseMoveEvent( e );
        updateGL();
    }
}

void GlDisplay::mousePressEvent ( QMouseEvent * e )
{
    qDebug("MousePress");
    ICommand* cmd = SPAPP->getMainWindow()->getSelectedCommand();
    
    if (cmd)
    {
        cmd->mousePressEvent( e );
        updateGL();
    }
}

void GlDisplay::mouseReleaseEvent ( QMouseEvent * e )
{
    qDebug("MouseRelease");
    ICommand* cmd = SPAPP->getMainWindow()->getSelectedCommand();
    
    if (cmd)
    {
        cmd->mouseReleaseEvent( e );
        updateGL();
    }
}

void GlDisplay::wheelEvent ( QWheelEvent * event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    m_zoomFactor += numSteps * 0.01;
    
    resizeGL( width(), height() );
    updateGL();
}

QVector<HitRecord> GlDisplay::getPickRecords(int x, int y)
{
    QVector<HitRecord> records;
    
    glRenderMode(GL_SELECT);
    glInitNames();
        
    glPushName(0);
        
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    GLint viewport[4] ={0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), 1.0f, 1.0f, viewport);
    
    glOrtho( -DEFAULT_HEIGHT / 2 * m_zoomFactor * m_aspectRatio,
              DEFAULT_HEIGHT / 2 * m_zoomFactor * m_aspectRatio,
             -DEFAULT_HEIGHT / 2 * m_zoomFactor,
              DEFAULT_HEIGHT / 2 * m_zoomFactor,
             -1000.0,
              1000.0 );

    /*
    int w = width();
    int h = height();
    glOrtho( (w % 2) == 0 ? -w/2 * m_zoomFactor: -(w+1)/2* m_zoomFactor,
             (w % 2) == 0 ? w/2 * m_zoomFactor: (w+1)/2 * m_zoomFactor,
             (h % 2) == 0 ? -h/2 * m_zoomFactor: -(h+1)/2* m_zoomFactor,
             (h % 2) == 0 ? h/2 * m_zoomFactor: (h+1)/2 * m_zoomFactor,
             -100.0,
             100.0 );
     */
    glMatrixMode(GL_MODELVIEW);
    
    drawObjects();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    int d = glRenderMode(GL_RENDER);

    //qDebug("Hits = %d", d);
    if (d > 0)
    {
        records.resize(d);
        for (int i = 0; i < d; i++)
        {
            //qDebug("%d : hits[0] = %d, hits[1] = %d, hits[2] = %d, hits[3] = %d",
            //       i, m_selectBuffer[i*4], m_selectBuffer[i*4 + 1], m_selectBuffer[i*4 + 2], m_selectBuffer[i*4 + 3]);
            records[i].numNamesInStack = m_selectBuffer[i*4];
            records[i].minDepth = m_selectBuffer[i*4 + 1];
            records[i].maxDepth = m_selectBuffer[i*4 + 2];
            records[i].stackContents = m_selectBuffer[i*4 + 3];
        }
    }
    
    return records;
}

Camera* GlDisplay::getViewCamera() 
{ 
    if (!m_cameraList.contains(m_viewType))
    {
        qDebug("camera %d does not exists. Returning perspective camera.", m_viewType);
        return m_cameraList[Perspective];
    }
    return m_cameraList[m_viewType];
}

