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
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/material/PointMaterial.h>

#define BO_POOL_NAME "PointRendererPool"

using core::subdivision::Vertex;

struct PointRenderer::Impl {
    Impl(const std::string & name = std::string(BO_POOL_NAME)) 
    : name(name), pointSize(3.0), colorSelected(1.f, 0, 0) 
    {
        
    }
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last draw.
     */
	void renderObject(std::shared_ptr<SceneNode> & node);

    /**
     *
     */
    VertexBuffer* getVBO(const ISurface* mesh);
    
    VAO* getVAO(ISurface* mesh);

    void fillVertexBuffer(const ISurface* mesh, VertexBuffer* vbo);

    std::string name;  /*< pool name of the VBO to create */
    float       pointSize;
    Vector3     colorSelected;
    
    static std::shared_ptr<PointMaterial>    mat;
};

std::shared_ptr<PointMaterial> PointRenderer::Impl::mat;

PointRenderer::PointRenderer() : d_(new Impl)
{
    std::cerr << "PointRenderer constructor" << std::endl;
}

PointRenderer::PointRenderer(const std::string & name) : d_(new Impl(name))
{
    std::cerr << "PointRenderer constructor" << std::endl;
}


PointRenderer::~PointRenderer()
{
    std::cerr << "PointRenderer destructor" << std::endl;
    BOManager::getInstance()->destroyPool(d_->name.c_str());
}

void PointRenderer::renderObject(std::shared_ptr<SceneNode> & node)
{
	d_->renderObject(node);
}

void PointRenderer::setPointSize(float size)
{
    d_->pointSize = size;
}

float PointRenderer::pointSize()
{
    return d_->pointSize;
}

void PointRenderer::Impl::renderObject(std::shared_ptr<SceneNode> & node)
{
	ISurface * obj = NULL;
	std::shared_ptr<Material> mat;
	
	if (!node) {
		return;
	}
	SurfaceNode::shared_ptr snode = std::dynamic_pointer_cast<SurfaceNode>(node);
	if (!snode) {
		std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
		return;
	}
	obj = snode->surface();
	mat = snode->material();
	
	//std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (obj == NULL)
        return;
    
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
		
		if (mat) {
			GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
			GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
			
			glEnableVertexAttribArray(attColor);
			glEnableVertexAttribArray(attVtx);
			
			glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), NULL);
			glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (const GLvoid*)(4*sizeof(GLfloat)));
		} else {
			std::cerr << " Material is NULL. Unable to bind attributes.\n";
		}
    }
	
    glPointSize(pointSize);
    if (mat) mat->shaderProgram()->useProgram();
    GLsizei nVertices = static_cast<GLsizei>(obj->numVertices());
    glDrawArrays(GL_POINTS, 0, nVertices);
    vao->release();

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

VAO* PointRenderer::Impl::getVAO(ISurface* mesh)
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

void PointRenderer::Impl::fillVertexBuffer(const ISurface* mesh, VertexBuffer* vbo)
{
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;

    size_t numFloats = numVertices*8;
    GLfloat* vtxData = new GLfloat[numFloats];

    Iterator<VertexHandle> it = mesh->constVertexIterator();
    int offset = 0;
    while(it.hasNext()) {
        auto v = static_cast<Vertex*>(it.next());
        vtxData[offset] = v->position().x();
        offset++;
        vtxData[offset] = v->position().y();
        offset++;
        vtxData[offset] = v->position().z();
        offset++;
        vtxData[offset] = 1.0f;
        offset++;

        if(v->flags() & VF_Selected) {
            vtxData[offset] = colorSelected.x();
            offset++;
            vtxData[offset] = colorSelected.y();
            offset++;
            vtxData[offset] = colorSelected.z();
            offset++;
            vtxData[offset] = 1.0f;
            offset++;
        } else {
            vtxData[offset] = v->color().r();
            offset++;
            vtxData[offset] = v->color().g();
            offset++;
            vtxData[offset] = v->color().b();
            offset++;
            vtxData[offset] = 1.0f;
            offset++;
        }

    }

    GLuint bufferSize = static_cast<GLuint>(numFloats*sizeof(GLfloat));
    vbo->setBufferData((GLvoid*)vtxData, bufferSize);

    delete [] vtxData;
}
