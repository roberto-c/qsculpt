/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
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
#include "StdAfx.h"
#include "SmoothRenderer.h"
#include "ISurface.h"
#include <QtOpenGL/QtOpenGL>
#include "BOManager.h"

#define BO_POOL_NAME "SmoothRendererPool"

static GLfloat g_selectedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
static GLfloat g_normalColor[] =   {0.8f, 0.8f, 0.8f, 1.0f};

typedef struct tagSmoothVtxStruct
{
    GLfloat v[3];
    GLfloat n[3];
    GLfloat color[4];
} SmoothVtxStruct;

struct SmoothRenderer::Impl {
    size_t numTriangles;
    
	/**
	 * Draw the mesh using OpenGL VBOs.
	 * The VBOs are re-build when the mesh has been changed since the last draw.
	 */
	void renderVbo(const ISurface* mesh);
	
	/**
	 * Draw the mesh using the glBeing()/glEnd() and friends functions.
	 * This method is a fallback method if the  VBOs are not supported.
	 */
	void renderImmediate(const ISurface* mesh);
	
	/**
	 *
	 */
	VertexBuffer* getVBO(ISurface* mesh);
	
	/**
	 * Fill vertex buffer with the mesh data.
	 */
	void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo);
    
    bool processPolygon(const Face & f, 
                        std::vector<SmoothVtxStruct> & buffer,
                        size_t & pos);
    
    bool processTriangle(const Vertex & v1,
                         const Vertex & v2,
                         const Vertex & v3,
                         std::vector<SmoothVtxStruct> & buffer,
                         size_t & pos,
                         GLfloat color[4]);
};

SmoothRenderer::SmoothRenderer() : _d(new Impl)
{
	qDebug() << "SmoothRenderer constructor";
}

SmoothRenderer::~SmoothRenderer()
{
	qDebug() << "SmoothRenderer destructor";
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void SmoothRenderer::renderObject(const ISurface* mesh)
{
	_d->renderVbo(mesh);
}

void SmoothRenderer::Impl::renderImmediate(const ISurface* mesh)
{	
	if (mesh->isSelected())
		glColor3d(0.0, 1.0, 0.0);
	else
		glColor3d(0.8, 0.8, 0.8);

    
    Iterator<Face> it = mesh->constFaceIterator();
	while(it.hasNext()) {
		const Face& f = it.next();
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        glBegin(GL_POLYGON);
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            glNormal3fv(v.normal().data());
            glVertex3fv(v.position().data());
            // qDebug() << "Vertex:" << toString(v.position());
        }
        glEnd();
	}
    
}

void SmoothRenderer::Impl::renderVbo(const ISurface* mesh)
{
	//qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
	if (mesh == NULL)
		return;
	
	ISurface* obj = const_cast<ISurface*>(mesh);
	VertexBuffer* vbo= getVBO(obj);
	if (vbo == NULL || vbo->getBufferID() == 0)
	{
		qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
		renderImmediate(mesh);
		return;
	}
	
	// Set the depth function to the correct value
	glDepthFunc(GL_LESS);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	
	if (vbo->needUpdate())
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
    glColorPointer(4, GL_FLOAT, sizeof(SmoothVtxStruct), (GLvoid*)offsetof(SmoothVtxStruct, color));
    glNormalPointer(GL_FLOAT, sizeof(SmoothVtxStruct), (GLvoid*)offsetof(SmoothVtxStruct, n));
    glVertexPointer(3, GL_FLOAT, sizeof(SmoothVtxStruct), (GLvoid*)offsetof(SmoothVtxStruct, v));
    	
	QColor color;
	color = Qt::white; //mesh->getPointList().at(f.normal[j]).color;
	if (mesh->isSelected())
	{
		glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
	}
	else
	{
		glColor3d(color.redF(), color.greenF(), color.blueF());
	}
	
    size_t numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

VertexBuffer* SmoothRenderer::Impl::getVBO(ISurface* mesh)
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;	
}

void SmoothRenderer::Impl::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    //qDebug() << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->getBufferID() == 0)
        return;
    
    int numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;
    
    Iterator<Face> it = mesh->constFaceIterator();
    numFaces = 0; // number of faces after triangulation
    while(it.hasNext()) {
        numFaces += it.next().numVertices() - 2;
    }
    
    size_t numVertices = numFaces*3;
    std::vector<SmoothVtxStruct> vtxData(numVertices); // Triangles
    
    size_t offset = 0;
    it = mesh->constFaceIterator();
    while(it.hasNext()) {
        processPolygon(it.next(), vtxData, offset);
    }
    // offset contains the number of vertices in the vtxData after being 
    // processed.
    vbo->setBufferData((GLvoid*)vtxData.data(), offset*sizeof(SmoothVtxStruct));
    
    //qDebug() << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}

bool SmoothRenderer::Impl::processPolygon(const Face & f, 
                                        std::vector<SmoothVtxStruct> & vtxData,
                                        size_t & offset)
{
    size_t nVtx = f.numVertices();
    if (nVtx < 3) {
        qDebug() << "Incomplete polygon. A polygon should have at least 3 vertices";
        return false;
    }
    GLfloat * color = f.flags() && FF_Selected ? g_selectedColor : g_normalColor;
    
    Iterator<Vertex> vtxIt = f.constVertexIterator();
    const Vertex * v1 = &(vtxIt.next());
    const Vertex * v2 = &(vtxIt.next());
    Vector3 n;
    while(vtxIt.hasNext()) {
        const Vertex * v3 = &(vtxIt.next());
        processTriangle(*v1, *v2, *v3, vtxData, offset, color);
        v2 = v3;
    }
    return true;
}

bool SmoothRenderer::Impl::processTriangle(const Vertex & v1,
                                         const Vertex & v2,
                                         const Vertex & v3,
                                         std::vector<SmoothVtxStruct> & vtxData,
                                         size_t & offset, 
                                         GLfloat color[4])
{
    vtxData[offset].v[0] = v1.position().x();
    vtxData[offset].v[1] = v1.position().y();
    vtxData[offset].v[2] = v1.position().z();
    vtxData[offset].n[0] = v1.normal().x();
    vtxData[offset].n[1] = v1.normal().y();
    vtxData[offset].n[2] = v1.normal().z();
    memcpy(vtxData[offset].color, color, sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v2.position().x();
    vtxData[offset].v[1] = v2.position().y();
    vtxData[offset].v[2] = v2.position().z();
    vtxData[offset].n[0] = v2.normal().x();
    vtxData[offset].n[1] = v2.normal().y();
    vtxData[offset].n[2] = v2.normal().z();
    memcpy(vtxData[offset].color, color, sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v3.position().x();
    vtxData[offset].v[1] = v3.position().y();
    vtxData[offset].v[2] = v3.position().z();
    vtxData[offset].n[0] = v3.normal().x();
    vtxData[offset].n[1] = v3.normal().y();
    vtxData[offset].n[2] = v3.normal().z();
    memcpy(vtxData[offset].color, color, sizeof(vtxData[offset].color)) ;
    offset++;
    
    return true;
}
