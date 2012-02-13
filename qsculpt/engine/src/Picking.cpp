/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores   *
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
#include "StdAfx.h"
#include <QtOpenGL/QtOpenGL>
#include "Picking.h"

#include "ISurface.h"
#include "BOManager.h"

#define BO_POOL_NAME			"ObjectPickingPool"
#define BO_VERTEX_POOL_NAME		"VertexPickingPool"

char* intToHexStr(unsigned int d);

PickingObjectRenderer::PickingObjectRenderer()
{
}

PickingObjectRenderer::~PickingObjectRenderer()
{
    qDebug() << "PickingObjectRenderer destructor";
    BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

char* intToHexStr(unsigned int d)
{
    const char* symbols = "0123456789ABCDEF";
    char* output = new char[34];
    memset(output, 0, 34*sizeof(char));

    unsigned int digit = 0, c = d;
    int index = 0;
    do
    {
        digit = c % 16;
        c = c / 16;
        output[index] = symbols[digit];
        index++;
    }while(c);

    // get the length of the string
    int len = 0;
    char* ptr = output;
    while(*ptr)
    { len++; ptr++;}

    // Reverse the order of the string
    for (int i =0; i < len/2; ++i)
    {
        char tmp = output[i];
        output[i] = output[len - i - 1];
        output[len - i - 1] = tmp;
    }
    return output;
}

void PickingObjectRenderer::renderObject(const ISurface* mesh, GLuint objId)
{
    renderVbo(mesh, objId);
}

void PickingObjectRenderer::renderImmediate(const ISurface* mesh, unsigned int /*objID*/)
{    
    mesh->lock();
    int numVertices = mesh->numVertices();
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

void PickingObjectRenderer::renderVbo(const ISurface* mesh, unsigned int objID)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;

    ISurface* obj = const_cast<ISurface*>(mesh);
    VertexBuffer *vbo = getVBO(obj);
    if (vbo == NULL || vbo->getBufferID() == 0)
    {
        qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
        renderImmediate(mesh, objID);
        return;
    }

    if (vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
    glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);

    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT|GL_LIGHTING_BIT|GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    glEnableClientState(GL_VERTEX_ARRAY);

    glPointSize(1.0f);
    glColor4ub( objID & 0xff,(objID>>8) & 0xff, (objID>>16) & 0xff, (objID>>24) & 0xff);
    glDrawArrays(GL_QUADS, 0, mesh->numFaces()*4);

    glPopAttrib();

    printGlError();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
}

VertexBuffer* PickingObjectRenderer::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

void PickingObjectRenderer::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    if (mesh == NULL || vbo->getBufferID() == 0)
        return;

    int numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    int numVertices = numFaces*4;
    int numFloats = numVertices*6;	// num floats = Faces * Num of point by face
    //				* Num elements of point
    GLfloat* vtxData = new GLfloat[numFloats]; // Make room for vertices and normals data

    int offset = 0;
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        const Face& f = it.next();
        //        qDebug() << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            qDebug() << "Vertex:" << toString(v.position());
            vtxData[offset++] = v.position().x();
            vtxData[offset++] = v.position().y();
            vtxData[offset++] = v.position().z();
            
            vtxData[offset++] = v.normal().x();
            vtxData[offset++] = v.normal().y();
            vtxData[offset++] = v.normal().z();
        }        
    }

    vbo->setBufferData((GLvoid*)vtxData, numFloats*sizeof(GLfloat));

    delete [] vtxData;
}
