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
#include <PlastilinaCore/PointRenderer.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Material.h>

#define BO_POOL_NAME "PointRendererPool"

struct PointRenderer::Impl {
    Impl(const std::string & name = std::string(BO_POOL_NAME)) 
    : name(name), pointSize(3.0), colorSelected(1.f, 0, 0) 
    {
        
    }
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last draw.
     */
    void renderVbo(const ISurface* mesh, const Material * mat);

    /**
     * Draw the mesh using the glBeing()/glEnd() and friends functions.
     * This method is a fallback method if the  VBOs are not supported.
     */
    void renderImmediate(const ISurface* mesh);

    /**
     *
     */
    VertexBuffer* getVBO(const ISurface* mesh);

    void fillVertexBuffer(const ISurface* mesh, VertexBuffer* vbo);

    std::string name;  /*< pool name of the VBO to create */
    float       pointSize;
    Vector3     colorSelected;
};

PointRenderer::PointRenderer() : d_(new Impl)
{
    std::cerr << "PointRenderer constructor";
}

PointRenderer::PointRenderer(const std::string & name) : d_(new Impl(name))
{
    std::cerr << "PointRenderer constructor";
}


PointRenderer::~PointRenderer()
{
    std::cerr << "PointRenderer destructor";
    BOManager::getInstance()->destroyPool(d_->name.c_str());
}

void PointRenderer::renderObject(const ISurface* mesh, const Material * mat)
{
    d_->renderVbo(mesh, mat);
}

void PointRenderer::setPointSize(float size)
{
    d_->pointSize = size;
}

float PointRenderer::pointSize()
{
    return d_->pointSize;
}

void PointRenderer::Impl::renderImmediate(const ISurface* mesh)
{

}

void PointRenderer::Impl::renderVbo(const ISurface* mesh, const Material * mat)
{
    //std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;

    ISurface* obj = const_cast<ISurface*>(mesh);
    VertexBuffer *vbo = getVBO(obj);
    if (vbo == NULL || vbo->objectID() == 0)
    {
        std::cerr << "Failed to create VBO. Fallback to immediate mode" ;
        renderImmediate(mesh);
        return;
    }

    if (vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo->objectID());
//    glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), NULL);
//    glColorPointer(3, GL_FLOAT, 6*sizeof(GLfloat), (const GLvoid*)(3*sizeof(GLfloat)));
//
//    glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_POINT_BIT);
//
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_COLOR_ARRAY);
//
//    glEnable(GL_POINT_SMOOTH);
    glPointSize(pointSize);
//    glColor3f(1.0f, 1.0f, 1.0f);
    GLsizei nVertices = static_cast<GLsizei>(obj->numVertices());
    glDrawArrays(GL_POINTS, 0, nVertices);

    //	glDepthFunc(GL_EQUAL);
    //	glPointSize(1.5f);
    //	glColor3f(1.0f, 1.0f, 1.0f);
    //	glDrawArrays(GL_POINTS, 0, obj->getPointList().size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_COLOR_ARRAY);

//    glPopAttrib();
}

VertexBuffer* PointRenderer::Impl::getVBO(const ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(name.c_str(), mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(name.c_str(), mesh);
    }
    return vbo;
}

void PointRenderer::Impl::fillVertexBuffer(const ISurface* mesh, VertexBuffer* vbo)
{
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;

    size_t numFloats = numVertices*6;
    GLfloat* vtxData = new GLfloat[numFloats];

    Iterator<Vertex> it = mesh->constVertexIterator();
    int offset = 0;
    while(it.hasNext()) {
        auto v = it.next();
        vtxData[offset] = v->position().x();
        offset++;
        vtxData[offset] = v->position().y();
        offset++;
        vtxData[offset] = v->position().z();
        offset++;

        if(v->flags() & VF_Selected) {
            vtxData[offset] = colorSelected.x();
            offset++;
            vtxData[offset] = colorSelected.y();
            offset++;
            vtxData[offset] = colorSelected.z();
            offset++;
        } else {
            vtxData[offset] = v->color().r();
            offset++;
            vtxData[offset] = v->color().g();
            offset++;
            vtxData[offset] = v->color().b();
            offset++;
        }

    }

    GLuint bufferSize = static_cast<GLuint>(numFloats*sizeof(GLfloat));
    vbo->setBufferData((GLvoid*)vtxData, bufferSize);

    delete [] vtxData;
}
