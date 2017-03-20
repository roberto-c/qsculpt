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
#include <PlastilinaCore/Stable.h>

#include <PlastilinaCore/Picking.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "ObjectPickingPool"
#define BO_VERTEX_POOL_NAME "VertexPickingPool"

using core::subdivision::Vertex;
using core::subdivision::Face;

char* intToHexStr(unsigned int d);

typedef struct tagVertexStruct
{
    GLfloat v[4];
    GLfloat c[4];
} VertexStruct;

VertexStruct vertexStructFromVector(Vector3& v, Color& c)
{
    VertexStruct ret;
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

PickingObjectRenderer::PickingObjectRenderer() {}

PickingObjectRenderer::~PickingObjectRenderer()
{
    std::cerr << "PickingObjectRenderer destructor";
    BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

char* intToHexStr(unsigned int d)
{
    const char* symbols = "0123456789ABCDEF";
    char*       output  = new char[34];
    memset(output, 0, 34 * sizeof(char));

    unsigned int digit = 0, c = d;
    int          index = 0;
    do
    {
        digit         = c % 16;
        c             = c / 16;
        output[index] = symbols[digit];
        index++;
    } while (c);

    // get the length of the string
    int   len = 0;
    char* ptr = output;
    while (*ptr)
    {
        len++;
        ptr++;
    }

    // Reverse the order of the string
    for (int i = 0; i < len / 2; ++i)
    {
        char tmp            = output[i];
        output[i]           = output[len - i - 1];
        output[len - i - 1] = tmp;
    }
    return output;
}

void PickingObjectRenderer::renderObject(std::shared_ptr<SceneNode>& node)
{
    renderVbo(node);
}

void PickingObjectRenderer::renderVbo(std::shared_ptr<SceneNode>& node)
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
                                  sizeof(VertexStruct),
                                  (GLvoid*)offsetof(VertexStruct, c));
        }
        GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
        if (attVtx >= 0)
        {
            glEnableVertexAttribArray(attVtx);
            glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(VertexStruct),
                                  (GLvoid*)offsetof(VertexStruct, v));
        }
        THROW_IF_GLERROR("Failed to get attribute");
    }
    mat->shaderProgram()->useProgram();
    GLsizei numVertices = vbo->getBufferSize() / sizeof(VertexStruct);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    vao->unbind();
}

VAO* PickingObjectRenderer::getVAO(ISurface* mesh)
{
    VAO* vao = NULL;
    vao      = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
    if (vao == NULL)
    {
        vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
    }
    return vao;
}

VertexBuffer* PickingObjectRenderer::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo               = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

void PickingObjectRenderer::fillVertexBuffer(ISurface*     mesh,
                                             VertexBuffer* vbo)
{
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    Color c = Color::fromUintRGBA(mesh->iid());

    std::vector<VertexStruct> vtxData(numFaces * 4);

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
            vtxData.push_back(vertexStructFromVector(v->position(), c));
        }
    }

    GLuint sizeBuffer =
        static_cast<GLuint>(vtxData.size() * sizeof(VertexStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), sizeBuffer);
}
