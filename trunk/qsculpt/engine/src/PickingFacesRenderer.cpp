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
#include "PickingFacesRenderer.h"

#include "ISurface.h"
#include "BOManager.h"

#define BO_POOL_NAME "PickingVertexRendererPool"
#define BO_FLAT_POOL_NAME "PickingVertexRendererPool_Flat"

typedef struct tagVtxStruct 
{
    GLfloat v[3];
    GLubyte color[4];
} VtxStruct;

typedef struct tagFlatVtxStruct 
{
    GLfloat v[3];
    GLfloat n[3];
    GLubyte color[4];
} FlatVtxStruct;

#define SIZE_OF_VERTEX (3*sizeof(GLfloat))
#define SIZE_OF_NORMAL (3*sizeof(GLfloat))
#define SIZE_OF_COLOR (4*sizeof(GLubyte))
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

PickingFacesRenderer::PickingFacesRenderer()
{
}
PickingFacesRenderer::~PickingFacesRenderer()
{
    //BOManager::getInstance()->destroyPool(BO_POOL_NAME);
    BOManager::getInstance()->destroyPool(BO_FLAT_POOL_NAME);
}

void PickingFacesRenderer::renderObject(const ISurface* mesh, GLuint objId)
{
    renderVbo(mesh, objId);
}

void PickingFacesRenderer::renderVbo(const ISurface* mesh, unsigned int objId)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;

    ISurface* obj = const_cast<ISurface*>(mesh);
    //VertexBuffer *vbo = getVBO(obj);
    VertexBuffer *flatVbo = getFlatVBO(obj);
    if (//vbo == NULL || vbo->getBufferID() == 0 ||
            flatVbo == NULL || flatVbo->getBufferID() == 0)
    {
        qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
        renderImmediate(mesh, objId);
        return;
    }

    if (/*vbo->needUpdate() || */flatVbo->needUpdate())
    {
        //fillVertexBuffer(obj, vbo, objId);
        fillFlatVertexBuffer(obj, flatVbo, objId);
        //vbo->setNeedUpdate(false);
        flatVbo->setNeedUpdate(false);
    }

    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, flatVbo->getBufferID());
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(FlatVtxStruct),
                   BUFFER_OFFSET(SIZE_OF_VERTEX + SIZE_OF_NORMAL));
    glNormalPointer(GL_FLOAT, sizeof(FlatVtxStruct), BUFFER_OFFSET(SIZE_OF_VERTEX));
    glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), NULL);

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    //glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    GLsizei nElements = static_cast<GLsizei>(obj->numFaces()*4);
    glDrawArrays(GL_QUADS, 0, nElements);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    //	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
    //	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VtxStruct), BUFFER_OFFSET(SIZE_OF_VERTEX));
    //	glVertexPointer(3, GL_FLOAT, sizeof(VtxStruct), NULL);
    //
    //	glEnableClientState(GL_VERTEX_ARRAY);
    //	glEnableClientState(GL_COLOR_ARRAY);
    //
    //	glPointSize(1.0f);
    //	//glColor3f(1.0f, 1.0f, 1.0f);
    //	glDepthFunc(GL_LEQUAL);
    //	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());
    //
    //	glBindBuffer(GL_ARRAY_BUFFER, 0);
    //	glDisableClientState(GL_VERTEX_ARRAY);
    //	glDisableClientState(GL_COLOR_ARRAY);

    glPopAttrib();
}

void PickingFacesRenderer::renderImmediate(const ISurface* mesh, unsigned int /*objID*/)
{    
    mesh->lock();
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;
    
    glBegin(GL_POINTS);
    Iterator<Vertex> it = mesh->constVertexIterator();
    while(it.hasNext()) {
        const Vertex& v = it.next();
        glVertex3fv(v.position().data());
    }
    glEnd();
    mesh->unlock();
}

VertexBuffer* PickingFacesRenderer::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

VertexBuffer* PickingFacesRenderer::getFlatVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_FLAT_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_FLAT_POOL_NAME, mesh);
    }
    return vbo;
}

void PickingFacesRenderer::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo, GLuint objId)
{
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;

    VtxStruct* vtxData = new VtxStruct[numVertices];

    Iterator<Vertex> it = mesh->constVertexIterator();
    int offset = 0;
    while(it.hasNext()) {
        const Vertex& v = it.next();
        vtxData[offset].v[0] = v.position().x();
        vtxData[offset].v[1] = v.position().y();
        vtxData[offset].v[2] = v.position().z();
        memcpy((void*)vtxData[offset].color, (const GLubyte*)&objId, sizeof(objId));
        offset++;
        objId++;
    }

    GLuint bufferSize = static_cast<GLuint>(numVertices*sizeof(VtxStruct));
    vbo->setBufferData((GLvoid*)vtxData, bufferSize);

    delete [] vtxData;
}

void PickingFacesRenderer::fillFlatVertexBuffer(ISurface* mesh, VertexBuffer* vbo, GLuint objId)
{
    if (mesh == NULL || vbo->getBufferID() == 0)
        return;

    size_t numFaces = mesh->numFaces(); //getFaceList().size();
    if (numFaces == 0)
        return;

    size_t numVertices = numFaces*4;
    FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
    
    int fcounter = 0;
    int offset = 0;
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        const Face& f = it.next();
        qDebug() << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            qDebug() << "Vertex:" << toString(v.position());
            vtxData[offset].v[0] = v.position().x();
            vtxData[offset].v[1] = v.position().y();
            vtxData[offset].v[2] = v.position().z();
            
            vtxData[offset].n[0] = v.normal().x();
            vtxData[offset].n[1] = v.normal().y();
            vtxData[offset].n[2] = v.normal().z();
            
            memcpy((void*)vtxData[offset].color, (const GLubyte*)&objId, sizeof(objId));
            offset++;
        }
        objId++;
    }
    
    GLuint bufferSize = static_cast<GLuint>(numVertices*sizeof(FlatVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData, bufferSize);

    delete [] vtxData;
}
