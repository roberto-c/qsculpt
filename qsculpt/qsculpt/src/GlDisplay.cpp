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
#include "GlDisplay.h"
#include <QtOpenGL>
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
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

int g_x = 0; int g_y = 0;

PickingObjectRenderer g_picking;

#define SELECT_BUFFER_SIZE 512
#define DEFAULT_HEIGHT 5.0f

GlDisplay::GlDisplay(DocumentView* _parent)
: QGLWidget(_parent),
m_isGridVisible(false),
m_areNormalsVisible(false),
m_selectBuffer(NULL),
m_aspectRatio(1.0),
m_viewType(Front),
m_drawingMode(Points),
m_renderer(NULL),
m_selectionRenderer(NULL),
m_cursorShape(None),
m_zoomFactor(1.0),
m_textureId(0)
{
	// Type of renderer used for displaying objects on the screen
	m_renderer = RendererFactory::getRenderer(m_drawingMode);
	
	// Type of renderer used for user object picking
	m_selectionRenderer = RendererFactory::getRenderer(Points);
    
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
	
    setCursor(Qt::CrossCursor);
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
	
    delete m_renderer;
    m_renderer = NULL;
	
	delete m_selectionRenderer;
	m_selectionRenderer = NULL;
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

void GlDisplay::setDrawingMode(DrawingMode mode){
    m_drawingMode = mode;
    delete m_renderer;
    m_renderer = RendererFactory::getRenderer(m_drawingMode);
}

void GlDisplay::initializeGL()
{
    // Set up the rendering context, define display lists etc.:
	
    //glClearColor( 0.4, 0.4, 0.4, 1.0 );
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glSelectBuffer(SELECT_BUFFER_SIZE, m_selectBuffer);
	
    if (glIsEnabled(GL_DEPTH_TEST) == GL_FALSE)
        qDebug("Depth buffer not enabled.");
    else
    {
        qDebug("Depth buffer enabled. Precision: %d", format().depthBufferSize());
    }
	
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
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
	
    glMatrixMode( GL_MODELVIEW );
	
    glGetIntegerv(GL_VIEWPORT, m_viewport);
}

void GlDisplay::paintGL()
{
    //Point3D point( 3, 2, 4 );
	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindTexture(GL_TEXTURE_2D, 0);
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
	
    drawCursor();
	
	glFlush();
    //drawOrientationAxis();
}

void GlDisplay::drawObjects()
{
	if (m_renderer == NULL)
		return;
	
	IObject3D* mesh;
    IDocument* doc= ((DocumentView*)parentWidget())->getDocument();
    int count = doc->getObjectsCount();
    for ( int i = 0; i < count; i++ )
    {
        mesh = doc->getObject(i);
		
		glPushMatrix();
		float x = 0.0f, y = 0.0f, z = 0.0f;
		mesh->getPosition(&x, &y, &z);
		glTranslatef(x, y, z);
		m_renderer->renderObject(mesh);
		glPopMatrix();
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
    if (m_cursorShape == None)
        return;
	
    switch(m_cursorShape)
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
	        glEnable(GL_TEXTURE_2D);
	        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	        glEnable(GL_BLEND);
	        
	        // Disable depth test so we dont modify previusly rendered information
	        if (depthTestState)
	        	glDisable( GL_DEPTH_TEST);
			
	        // Store the current matrix to restore it later
	        glPushMatrix();
	        
	        // Start from a clean transformation matrix
	        glLoadIdentity();
	        
	        // Setup the texture and color
	        glBindTexture(GL_TEXTURE_2D, m_textureId);
	        glColor3f(1.0f, 1.0f, 0.0f);
			
	        // Calculate the coordinates of the box to paint the bitmap
	        double wx1, wy1, wz1;
	        mapScreenCoordsToWorldCoords(m_cursorPosition.getX() - m_cursorImage.width()/2,
										 m_cursorPosition.getY() - m_cursorImage.height() / 2,
										 m_cursorPosition.getZ(), &wx1, &wy1, &wz1);
	        double wx2, wy2, wz2;
	        mapScreenCoordsToWorldCoords(m_cursorPosition.getX() + m_cursorImage.width()/2,
										 m_cursorPosition.getY() + m_cursorImage.height() / 2,
										 m_cursorPosition.getZ(), &wx2, &wy2, &wz2);
			
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
	        	glDisable(GL_BLEND);
	    	if (textureState == false)
	        	glDisable(GL_TEXTURE_2D);
	    	if (depthTestState)
	    		glEnable( GL_DEPTH_TEST);
	    }
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
				  camera->getOrientationVector().getX(),
				  camera->getOrientationVector().getY(),
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
	
    bool needUpdate = false;
    if (cmd)
    {
        cmd->mouseMoveEvent( e );
        needUpdate = true;
    }
    if (m_cursorShape != None)
    {
    	m_cursorPosition.setPoint(e->x(), e->y(), 0);
    	needUpdate = true;
    }
    if (needUpdate)
    {
    	updateGL();
    }
}

void GlDisplay::mousePressEvent ( QMouseEvent * e )
{
    //qDebug("MousePress");
    ICommand* cmd = SPAPP->getMainWindow()->getSelectedCommand();
	
    bool needUpdate = false;
    if (cmd)
    {
        cmd->mousePressEvent( e );
        needUpdate = true;
    }
	m_cursorPosition.setX(e->x());
	m_cursorPosition.setY(e->y());
    if (m_cursorShape != None)
    {
		m_cursorPosition.setX(e->x());
    	m_cursorPosition.setY(e->y());
    	needUpdate = true;
    }
    if (needUpdate)
    {
    	updateGL();
    }
}

void GlDisplay::mouseReleaseEvent ( QMouseEvent * e )
{
    //qDebug("MouseRelease");
    ICommand* cmd = SPAPP->getMainWindow()->getSelectedCommand();
	
    if (cmd)
    {
        cmd->mouseReleaseEvent( e );
        updateGL();
    }
}

void GlDisplay::wheelEvent ( QWheelEvent * e )
{
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;
	
    m_zoomFactor += numSteps * 0.01;
	
    resizeGL( width(), height() );
    updateGL();
}

QVector<HitRecord> GlDisplay::getPickRecords(int _x, int _y)
{	
    QVector<HitRecord> records;
	
	ObjectContainer listObj = getSelectedObjects( _x, m_viewport[3] - _y);

	int d = listObj.size();
    if (d > 0)
    {
        records.resize(d);
        for (int i = 0; i < d; i++)
        {
            records[i].numNamesInStack = d;
            records[i].minDepth = 0;
            records[i].maxDepth = 0;
            records[i].stackContents = d;
        }
    }
    return records;
}

ObjectContainer GlDisplay::getSelectedObjects(GLint x, GLint y)
{
	ObjectContainer l;
	if (m_renderer == NULL)
		return l;
	
	unsigned int objId = 1;
	GLuint d = 0;
	IObject3D* mesh;
    IDocument* doc= ((DocumentView*)parentWidget())->getDocument();
    int count = doc->getObjectsCount();
    for ( int i = 0; i < count; i++ )
    {
        mesh = doc->getObject(i);
		
		glPushMatrix();
		float px = 0.0f, py = 0.0f, pz = 0.0f;
		mesh->getPosition(&px, &py, &pz);
		glTranslatef(px, py, pz);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		g_picking.renderObject(mesh, objId);
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)&d);
		
		if (d == objId)
			l.append(mesh);
		
		objId++;
		
		glPopMatrix();
		if (printGlError()) break;
    }
	return l;
}

Camera* GlDisplay::getViewCamera()
{
    if (!m_cameraList.contains(m_viewType))
    {
        //qDebug("camera %d does not exists. Returning perspective camera.", m_viewType);
        return m_cameraList[Perspective];
    }
    return m_cameraList[m_viewType];
}

void GlDisplay::set3DCursorShape(CursorShapeType shape)
{
    m_cursorShape = shape;
	
    // Turn on mouse tracking only if we are going to draw a cursor
    setMouseTracking(m_cursorShape != None);
}

void GlDisplay::setCursorImage(const QImage& image)
{
    if (image.isNull())
    {
    	qDebug("GlDisplay::setCursorImage: image is null !!!");
    }
    else
    {
    	// Delete any texture loaded before.
    	deleteTexture(m_textureId);
    	m_textureId = 0;
		
	    m_cursorImage = image;
    	m_textureId = bindTexture(m_cursorImage);
    	if (m_textureId == 0)
    	{
    		qDebug("texture id is not valid");
    	}
    }
	
}

/**
 * Returns a copy of the image used as cursor.
 */
QImage GlDisplay::getCursorImage()
{
	return m_cursorImage;
}

void GlDisplay::mapScreenCoordsToWorldCoords(int x, int y, int z, double *wx,
											 double *wy, double *wz)
{
    double modelMatrix[16], projMatrix[16];
    GLint viewPort[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewPort);
	
    gluUnProject( x, viewPort[3] - y, z, modelMatrix, projMatrix, viewPort,
				 (GLdouble*)wx, (GLdouble*)wy, (GLdouble*)wz);
}

void GlDisplay::mapWorldCoordsToScreenCoords(double wx, double wy, double wz,
											 int *x, int *y, int *z)
{
    double modelMatrix[16], projMatrix[16];
    GLint viewPort[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewPort);
	
    double dx, dy, dz;
    gluProject( (GLdouble)wx, (GLdouble)wy, (GLdouble)wz, 
			   modelMatrix, projMatrix, viewPort, &dx, &dy, &dz);
    if (x) *x = (int)dx;
    if (y) *y = (int)dy;
    if (z) *z = (int)dz;
}
