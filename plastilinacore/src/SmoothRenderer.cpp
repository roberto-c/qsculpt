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
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SmoothRenderer.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "SmoothRendererPool"

using core::subdivision::Vertex;
using core::subdivision::Face;

typedef struct tagSmoothVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat color[4];
} SmoothVtxStruct;

struct SmoothRenderer::Impl
{
    size_t numTriangles;

    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last
     * draw.
     */
    void renderObject(std::shared_ptr<SceneNode>& node);

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

    bool processPolygon(const Face& f, std::vector<SmoothVtxStruct>& buffer,
                        size_t& pos);

    bool processTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3,
                         std::vector<SmoothVtxStruct>& buffer, size_t& pos);
};

SmoothRenderer::SmoothRenderer()
    : _d(new Impl)
{
    std::cerr << "SmoothRenderer constructor" << std::endl;
}

SmoothRenderer::~SmoothRenderer()
{
    std::cerr << "SmoothRenderer destructor" << std::endl;
    BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void SmoothRenderer::renderObject(std::shared_ptr<SceneNode>& node)
{
    _d->renderObject(node);
}

void SmoothRenderer::Impl::renderObject(std::shared_ptr<SceneNode>& node)
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

    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);

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
                                  sizeof(SmoothVtxStruct),
                                  (GLvoid*)offsetof(SmoothVtxStruct, color));
        }
        GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
        if (attVtx >= 0)
        {
            glEnableVertexAttribArray(attVtx);
            glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(SmoothVtxStruct),
                                  (GLvoid*)offsetof(SmoothVtxStruct, v));
        }
        GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
        if (attNormal >= 0)
        {
            glEnableVertexAttribArray(attNormal);
            glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(SmoothVtxStruct),
                                  (GLvoid*)offsetof(SmoothVtxStruct, n));
        }
        THROW_IF_GLERROR("Failed to get attribute");
    }
    mat->shaderProgram()->useProgram();
    GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    vao->unbind();
}

VertexBuffer* SmoothRenderer::Impl::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo               = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

VAO* SmoothRenderer::Impl::getVAO(ISurface* mesh)
{
    VAO* vao = NULL;
    vao      = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
    if (vao == NULL)
    {
        vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
    }
    return vao;
}

void SmoothRenderer::Impl::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    // std::cerr << "FlatRenderer::fillVertexBuffer Start time:" <<
    // QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    Iterator<FaceHandle> it = mesh->constFaceIterator();
    numFaces                = 0; // number of faces after triangulation
    while (it.hasNext())
    {
        numFaces += static_cast<Face*>(it.next())->numVertices() - 2;
    }

    size_t                       numVertices = numFaces * 3;
    std::vector<SmoothVtxStruct> vtxData(numVertices); // Triangles

    size_t offset = 0;
    it            = mesh->constFaceIterator();
    while (it.hasNext())
    {
        auto f = static_cast<Face*>(it.next());
        processPolygon(*f, vtxData, offset);
    }
    // offset contains the number of vertices in the vtxData after being
    // processed.
    GLuint dataSize = static_cast<GLuint>(offset * sizeof(SmoothVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), dataSize);
    THROW_IF_GLERROR(__func__);

    // std::cerr << "FlatRenderer::fillVertexBuffer End time:" <<
    // QDateTime::currentDateTime();
}

bool SmoothRenderer::Impl::processPolygon(
    const Face& f, std::vector<SmoothVtxStruct>& vtxData, size_t& offset)
{
    size_t nVtx = f.numVertices();
    if (nVtx < 3)
    {
        std::cerr
            << "Incomplete polygon. A polygon should have at least 3 vertices"
            << std::endl;
        return false;
    }
    // GLfloat * color = f.flags() && FF_Selected ? g_selectedColor :
    // g_normalColor;

    Iterator<VertexHandle> vtxIt = f.constVertexIterator();
    auto                   v1    = static_cast<Vertex*>(vtxIt.next());
    auto                   v2    = static_cast<Vertex*>(vtxIt.next());
    Vector3                n;
    while (vtxIt.hasNext())
    {
        auto v3 = static_cast<Vertex*>(vtxIt.next());
        processTriangle(*v1, *v2, *v3, vtxData, offset);
        v2 = v3;
    }
    return true;
}

bool SmoothRenderer::Impl::processTriangle(
    const Vertex& v1, const Vertex& v2, const Vertex& v3,
    std::vector<SmoothVtxStruct>& vtxData, size_t& offset)
{
    vtxData[offset].v[0] = v1.position().x();
    vtxData[offset].v[1] = v1.position().y();
    vtxData[offset].v[2] = v1.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v1.normal().x();
    vtxData[offset].n[1] = v1.normal().y();
    vtxData[offset].n[2] = v1.normal().z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v1.color().data().data(),
           sizeof(vtxData[offset].color));
    offset++;

    vtxData[offset].v[0] = v2.position().x();
    vtxData[offset].v[1] = v2.position().y();
    vtxData[offset].v[2] = v2.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v2.normal().x();
    vtxData[offset].n[1] = v2.normal().y();
    vtxData[offset].n[2] = v2.normal().z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v2.color().data().data(),
           sizeof(vtxData[offset].color));
    offset++;

    vtxData[offset].v[0] = v3.position().x();
    vtxData[offset].v[1] = v3.position().y();
    vtxData[offset].v[2] = v3.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v3.normal().x();
    vtxData[offset].n[1] = v3.normal().y();
    vtxData[offset].n[2] = v3.normal().z();
    vtxData[offset].n[3] = 0;
    memcpy(vtxData[offset].color, v3.color().data().data(),
           sizeof(vtxData[offset].color));
    offset++;

    return true;
}
