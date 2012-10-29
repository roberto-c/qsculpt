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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/FlatRenderer.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/BOManager.h>
#include <cstddef>
#include <iostream>
#include <PlastilinaCore/GlslProgram.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/Material.h>

using namespace std;

#define BO_POOL_NAME "FlatRendererPool"

#define SIZE_OF_VERTEX (3*sizeof(GLfloat))
#define SIZE_OF_NORMAL (3*sizeof(GLfloat))
#define SIZE_OF_COLOR (4*sizeof(GLfloat))
//#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)
#define BUFFER_OFFSET(start, elem) ((GLubyte*)elem - (GLubyte*)start)

bool g_enableVBO = false;

GLfloat g_selectedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat g_normalColor[] =   {0.8f, 0.8f, 0.8f, 1.0f};

typedef struct tagFlatVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat color[4];
} FlatVtxStruct;

struct FlatRenderer::Impl {
    size_t numTriangles;
    
    GlslProgram * shaderProgram;
    
    Impl() : numTriangles(0), shaderProgram(NULL){}
    
	/**
	 * Draw the mesh using OpenGL VBOs.
	 * The VBOs are re-build when the mesh has been changed since the last draw.
	 */
	void renderVbo(const ISurface* mesh, const Material * mat);
	
	/**
	 *
	 */
	VertexBuffer* getVBO(ISurface* mesh);
    
    /**
	 *
	 */
	VAO* getVAO(ISurface* mesh);
	
	/**
	 * Fill vertex buffer with the mesh data.
	 */
	void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo);
    
    bool processPolygon(const Face & f, 
                        std::vector<FlatVtxStruct> & buffer,
                        size_t & pos);
    
    bool processTriangle(const Vertex & v1,
                         const Vertex & v2,
                         const Vertex & v3,
                         std::vector<FlatVtxStruct> & buffer,
                         size_t & pos);
};

FlatRenderer::FlatRenderer() : _d(new Impl)
{
    cerr << "FlatRenderer constructor" << endl;
}

FlatRenderer::~FlatRenderer()
{
    cerr << "FlatRenderer destructor" << endl;
    BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void FlatRenderer::setShaderProgram(GlslProgram * shader)
{
    _d->shaderProgram = shader;
}

void FlatRenderer::renderObject(const ISurface* mesh, const Material * mat)
{
    _d->renderVbo(mesh, mat);
}

void FlatRenderer::Impl::renderVbo(const ISurface* mesh, const Material * mat)
{    
    //std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;

    
	ISurface* obj = const_cast<ISurface*>(mesh);
	VertexBuffer* vbo= getVBO(obj);
	if (vbo == NULL || vbo->objectID() == 0)
	{
		std::cerr << "Failed to create VBO."  << std::endl;
		return;
	}
    VAO* vao = getVAO(obj);
    if (vao == NULL || vao->objectID() == 0)
	{
		std::cerr << "Failed to create VAO."  << std::endl;
		return;
	}
    
    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);
    
    vao->bind();
    vbo->bind();
    if (vbo->needUpdate())
	{
		fillVertexBuffer(obj, vbo);
		vbo->setNeedUpdate(false);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, v));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, n));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, color));
    }
    
    mat->shaderProgram()->useProgram();
    GLsizei numVertices = vbo->getBufferSize() / sizeof(FlatVtxStruct);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    vao->release();
}

VertexBuffer* FlatRenderer::Impl::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

VAO* FlatRenderer::Impl::getVAO(ISurface* mesh)
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

void FlatRenderer::Impl::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    //std::cerr << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    Iterator<Face> it = mesh->constFaceIterator();
    numFaces = 0; // number of faces after triangulation
    while(it.hasNext()) {
        auto face = it.next();
        if (face) {
            numFaces += face->numVertices() - 2;
        }
    }

    size_t numVertices = numFaces*3;
    std::vector<FlatVtxStruct> vtxData(numVertices); // Triangles

    size_t offset = 0;
    it = mesh->constFaceIterator();
    while(it.hasNext()) {
        auto f = it.next();
        processPolygon(*f, vtxData, offset);
    }
    // offset contains the number of vertices in the vtxData after being 
    // processed.
    GLuint bufferSize = static_cast<GLuint>(offset*sizeof(FlatVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), bufferSize);

    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}

bool FlatRenderer::Impl::processPolygon(const Face & f, 
                                        std::vector<FlatVtxStruct> & vtxData,
                                        size_t & offset)
{
    size_t nVtx = f.numVertices();
    if (nVtx < 3) {
        std::cerr << "Incomplete polygon. A polygon should have at least 3 vertices"  << std::endl;
        return false;
    }
    
    Iterator<Vertex> vtxIt = f.constVertexIterator();
    auto v1 = vtxIt.next();
    auto v2 = vtxIt.next();
    Vector3 n;
    while(vtxIt.hasNext()) {
        auto v3 = vtxIt.next();
        processTriangle(*v1, *v2, *v3, vtxData, offset);
        v2 = v3;
    }
    return true;
}

bool FlatRenderer::Impl::processTriangle(const Vertex & v1,
                                         const Vertex & v2,
                                         const Vertex & v3,
                                         std::vector<FlatVtxStruct> & vtxData,
                                         size_t & offset)
{
    Vector3 normal = (v2.position() - v1.position()).cross(v3.position() - v1.position());
    normal.normalize();
    vtxData[offset].v[0] = v1.position().x();
    vtxData[offset].v[1] = v1.position().y();
    vtxData[offset].v[2] = v1.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = normal.x();
    vtxData[offset].n[1] = normal.y();
    vtxData[offset].n[2] = normal.z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v1.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v2.position().x();
    vtxData[offset].v[1] = v2.position().y();
    vtxData[offset].v[2] = v2.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = normal.x();
    vtxData[offset].n[1] = normal.y();
    vtxData[offset].n[2] = normal.z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v2.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v3.position().x();
    vtxData[offset].v[1] = v3.position().y();
    vtxData[offset].v[2] = v3.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = normal.x();
    vtxData[offset].n[1] = normal.y();
    vtxData[offset].n[2] = normal.z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v3.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    return true;
}