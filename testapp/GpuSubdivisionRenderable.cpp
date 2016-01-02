/***************************************************************************
 *   Copyright (C) 2013 by Juan Roberto Cabral Flores                      *
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
#include "stable.h"
#include "GpuSubdivisionRenderable.h"

#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/RenderState.h>
#include <PlastilinaCore/SmoothRenderer.h>
#include <PlastilinaCore/Scene.h>

#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "SmoothRendererPool"

struct SmoothVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat c[4];
    GLfloat t[2];
};

namespace core {
using core::subdivision::Vertex;
using core::subdivision::Face;
using core::subdivision::Edge;

struct GpuSubdivisionRenderable::Impl {
    const core::GpuSubdivision * surface;
    
    Impl() {}
    
    void renderObject(RenderState & state) const;
    
    VertexBuffer* getVBO(ISurface* mesh) const;
    
    VAO* getVAO(ISurface* mesh) const;
    
    void fillVertexBuffer(ISurface* mesh,
        VertexBuffer* vbo) const;
};

GpuSubdivisionRenderable::GpuSubdivisionRenderable(
    const core::GpuSubdivision * surface)
: _d(new Impl())
{
    _d->surface = surface;
}

GpuSubdivisionRenderable::~GpuSubdivisionRenderable()
{
    
}

void GpuSubdivisionRenderable::render(
    RenderState & state) const
{
    _d->renderObject(state);
}
    

void GpuSubdivisionRenderable::Impl::renderObject(
    RenderState & state) const
{
    ISurface * obj = NULL;
	std::shared_ptr<Material> mat;
	
	auto node = state.currentNode;
	if (!node) {
		return;
	}
	auto snode = std::dynamic_pointer_cast<const SurfaceNode>(node);
	if (!snode) {
		std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
		return;
	}
	
	if (snode->material() && snode->material()->shaderProgram()) {
		snode->material()->shaderProgram()->useProgram();
		snode->material()->setup(state.root);
	}
	
	GlslProgram * prog = GlslProgram::currentProgram();
	if (prog->programID() > 0) {
		auto cameraNode = state.camera;
        Camera * camera = cameraNode->camera().get();
		GLint matId = prog->uniformLocation("glModelViewMatrix");
        Eigen::Affine3f t(camera->modelView());
//        std::cout << "ModelView: " << std::endl << t.matrix() << std::endl;
        if (matId != -1) prog->setUniform(matId, t);
        
        matId = prog->uniformLocation("objectTransform");
        auto ntrans = node->transform()*node->parentTransform();
//        std::cout << "Object Trans: " << std::endl << ntrans.matrix() << std::endl;
        if (matId != -1) prog->setUniform(matId, ntrans);
        
        matId = prog->uniformLocation("glProjectionMatrix");
//        std::cout << "Projection: " << std::endl << camera->projection() << std::endl;
        if (matId != -1) prog->setUniform(matId, camera->projection());
        
        Eigen::Vector3f p = t.translation();
		Eigen::Vector4f camPos = Eigen::Vector4f(p[0],p[1],p[2],1.0f);
		matId = prog->uniformLocation("eyePosition");
//        std::cout << "Eye position: " << std::endl << camPos << std::endl;
		if (matId != -1) prog->setUniform(matId, camPos);
	}
	
	
	obj = snode->surface();
	mat = snode->material();
	//std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
	if (obj == NULL || mat == NULL)
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
		
		GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
		if (attColor >= 0) {
			glEnableVertexAttribArray(attColor);
			glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
								  sizeof(SmoothVtxStruct),
								  (GLvoid*)offsetof(SmoothVtxStruct, c));
		}
		GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
		if (attVtx >= 0) {
			glEnableVertexAttribArray(attVtx);
			glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
								  sizeof(SmoothVtxStruct),
								  (GLvoid*)offsetof(SmoothVtxStruct, v));
		}
		GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
		if (attNormal >= 0) {
			glEnableVertexAttribArray(attNormal);
			glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
								  sizeof(SmoothVtxStruct),
								  (GLvoid*)offsetof(SmoothVtxStruct, n));
		}
        GLint attTexCoord = mat->shaderProgram()->attributeLocation("glTexCoord");
		if (attTexCoord >= 0) {
			glEnableVertexAttribArray(attTexCoord);
			glVertexAttribPointer(attTexCoord, 2, GL_FLOAT, GL_FALSE,
								  sizeof(SmoothVtxStruct),
								  (GLvoid*)offsetof(SmoothVtxStruct, t));
		}
		THROW_IF_GLERROR("Failed to get attribute");
    }
	
	switch (state.renderMode) {
		case RenderMode::RM_Smooth:
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			mat->shaderProgram()->useProgram();
			GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		}
			break;
			
		case RenderMode::RM_WireFrame:
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mat->shaderProgram()->useProgram();
			GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		}
			break;
		case RenderMode::RM_Points:
		{
			mat->shaderProgram()->useProgram();
			glPointSize(3.0f);
			GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
			glDrawArrays(GL_POINTS, 0, numVertices);
		}
			
		default:
			break;
	}
    
    
    vao->release();
}

VertexBuffer*
GpuSubdivisionRenderable::Impl::getVBO(
    ISurface* mesh) const
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

VAO*
GpuSubdivisionRenderable::Impl::getVAO(
    ISurface* mesh) const
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

void
GpuSubdivisionRenderable::Impl::fillVertexBuffer(ISurface* mesh,
    VertexBuffer* vbo) const
{
    //std::cerr << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    size_t numVertices = numFaces * 4;
    std::vector<SmoothVtxStruct> vtxData(numVertices); // Triangles

    size_t offset = 0;
    auto it = mesh->constFaceIterator();
    while (it.hasNext()) {
        auto f = static_cast<Face*>(it.next());
        auto vtxIt = f->constVertexIterator();
        while(vtxIt.hasNext()) 
        {
            auto v = static_cast<Vertex*>(vtxIt.next());
            vtxData[offset].v[0] = v->position()[0]; vtxData[offset].v[1] = v->position()[1]; vtxData[offset].v[2] = v->position()[2]; vtxData[offset].v[3] = 1.0f;
            vtxData[offset].n[0] = v->normal()[0]; vtxData[offset].n[1] = v->normal()[1]; vtxData[offset].n[2] = v->normal()[2]; vtxData[offset].n[3] = 0.0f;
            offset++;
        }
    }
    GLuint dataSize = static_cast<GLuint>(offset*sizeof(SmoothVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), dataSize);
    THROW_IF_GLERROR(__func__);
    
    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}
    
}; // namspace core
