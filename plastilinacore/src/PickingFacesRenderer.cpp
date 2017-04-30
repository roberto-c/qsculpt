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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/PickingFacesRenderer.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>

#define BO_POOL_NAME "PickingVertexRendererPool"
#define BO_FLAT_POOL_NAME "PickingVertexRendererPool_Flat"

using core::subdivision::Vertex;
using core::subdivision::Face;

typedef struct tagVtxStruct
{
    GLfloat v[4];
    GLfloat c[4];
} VtxStruct;

typedef struct tagFlatVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat c[4];
} FlatVtxStruct;

static VtxStruct vertexStructFromVector(Vector3& v, Color& c)
{
    VtxStruct ret;
    ret.v[0] = v[0];
    ret.v[1] = v[1];
    ret.v[2] = v[2];
    ret.v[3] = 1.0;
    ret.c[0] = c.data()[0];
    ret.c[1] = c.data()[1];
    ret.c[2] = c.data()[2];
    ret.c[3] = c.data()[3];
    return ret;
}

static FlatVtxStruct flatStructFromVector(Vector3& v, Vector3& n, Color& c)
{
    FlatVtxStruct ret;
    ret.v[0] = v[0];
    ret.v[1] = v[1];
    ret.v[2] = v[2];
    ret.v[3] = 1.0;
    ret.n[0] = n[0];
    ret.n[1] = n[1];
    ret.n[2] = n[2];
    ret.n[3] = 1.0;
    ret.c[0] = c.data()[0];
    ret.c[1] = c.data()[1];
    ret.c[2] = c.data()[2];
    ret.c[3] = c.data()[3];
    return ret;
}

#define SIZE_OF_VERTEX (4 * sizeof(GLfloat))
#define SIZE_OF_NORMAL (4 * sizeof(GLfloat))
#define SIZE_OF_COLOR (4 * sizeof(GLfloat))
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

PickingFacesRenderer::PickingFacesRenderer() {}
PickingFacesRenderer::~PickingFacesRenderer()
{
    // BOManager::getInstance()->destroyPool(BO_POOL_NAME);
    BOManager::getInstance()->destroyPool(BO_FLAT_POOL_NAME);
}

void PickingFacesRenderer::renderObject(std::shared_ptr<SceneNode>& node)
{
    ISurface*                 obj = NULL;
    std::shared_ptr<Material> mat;

    if (!node)
    {
        return;
    }
    SurfaceNode::shared_ptr snode =
        std::dynamic_pointer_cast<SurfaceNode>(node);
    if (!snode)
    {
        std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
        return;
    }
    obj = snode->surface();
    mat = snode->material();
    // std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (obj == NULL || mat == NULL)
        return;

    VertexBuffer* vbo = getVBO(obj);
    if (vbo == NULL || vbo->objectID() == 0)
    {
        std::cerr << "Failed to create VBO." << std::endl;
        return;
    }
    VAO* vao = getVAO(obj);
    if (vao == NULL || vao->objectID() == 0)
    {
        std::cerr << "Failed to create VAO." << std::endl;
        return;
    }

    vao->bind();
    vbo->bind();
    if (vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);

        GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
        if (attColor >= 0)
        {
            glEnableVertexAttribArray(attColor);
            glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, c));
        }
        GLint attNor = mat->shaderProgram()->attributeLocation("glNormal");
        if (attNor >= 0)
        {
            glEnableVertexAttribArray(attNor);
            glVertexAttribPointer(attNor, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, v));
        }
        GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
        if (attVtx >= 0)
        {
            glEnableVertexAttribArray(attVtx);
            glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, v));
        }
        THROW_IF_GLERROR("Failed to get attribute");
    }
    mat->shaderProgram()->useProgram();
    GLsizei numVertices = static_cast<GLsizei>(vbo->getBufferSize() / sizeof(FlatVtxStruct));
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    vao->unbind();
}

VAO* PickingFacesRenderer::getVAO(ISurface* mesh)
{
    VAO* vao = NULL;
    vao      = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
    if (vao == NULL)
    {
        vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
    }
    return vao;
}

VertexBuffer* PickingFacesRenderer::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo               = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
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

void PickingFacesRenderer::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;

    std::vector<VtxStruct> vtxData(numVertices);
    Color                  c  = Color::fromUintRGBA(mesh->iid());
    Iterator<FaceHandle>   it = mesh->constFaceIterator();
    while (it.hasNext())
    {
        auto f = static_cast<Face*>(it.next());
        //        std::cerr << "face " << fcounter++;
        Iterator<VertexHandle> vtxIt = f->constVertexIterator();
        while (vtxIt.hasNext())
        {
            auto v = static_cast<Vertex*>(vtxIt.next());
            std::cerr << "Vertex:" << toString(v->position());
            vtxData.push_back(vertexStructFromVector(v->position(), c));
        }
    }

    GLuint sizeBuffer =
        static_cast<GLuint>(vtxData.size() * sizeof(VtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), sizeBuffer);
}

void PickingFacesRenderer::fillFlatVertexBuffer(ISurface*     mesh,
                                                VertexBuffer* vbo)
{
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces(); // getFaceList().size();
    if (numFaces == 0)
        return;

    std::vector<FlatVtxStruct> vtxData(numFaces * 4);

    Color                c  = Color::fromUintRGBA(mesh->iid());
    Iterator<FaceHandle> it = mesh->constFaceIterator();
    while (it.hasNext())
    {
        auto f = static_cast<Face*>(it.next());
        //        std::cerr << "face " << fcounter++;
        Iterator<VertexHandle> vtxIt = f->constVertexIterator();
        while (vtxIt.hasNext())
        {
            auto v = static_cast<Vertex*>(vtxIt.next());
            std::cerr << "Vertex:" << toString(v->position());
            vtxData.push_back(
                flatStructFromVector(v->position(), v->normal(), c));
        }
    }

    GLuint sizeBuffer =
        static_cast<GLuint>(vtxData.size() * sizeof(FlatVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), sizeBuffer);
}
