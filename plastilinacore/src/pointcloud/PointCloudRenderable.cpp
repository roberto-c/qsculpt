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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/pointcloud/PointCloudRenderable.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/RenderState.h>
#include <PlastilinaCore/SmoothRenderer.h>
#include <PlastilinaCore/Scene.h>

#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "PointRendererPool"

using core::subdivision::Vertex;
using core::subdivision::Face;

struct PointVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat color[4];
    GLfloat t[2];
}; //SmoothVtxStruct;


PointCloudRenderable::PointCloudRenderable()
{
	std::cerr << "PointCloudRenderable constructor" << std::endl;
}

PointCloudRenderable::~PointCloudRenderable()
{
	std::cerr << "PointCloudRenderable destructor" << std::endl;
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void PointCloudRenderable::renderObject(RenderState & state) const
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
        if (matId != -1) prog->setUniform(matId, camera->modelView());
        matId = prog->uniformLocation("glProjectionMatrix");
        if (matId != -1) prog->setUniform(matId, camera->projection());
		auto p = cameraNode->localToWorld(camera->getPosition());
		Eigen::Vector4f camPos = Eigen::Vector4f(p[0],p[1],p[2],1.0f);
		matId = prog->uniformLocation("eyePosition");
		if (matId != -1) prog->setUniform(matId, camPos);
	}
	
	GLint matId = prog->uniformLocation("objectTransform");
	if (matId != -1) {
		prog->setUniform(matId, node->transform()*node->parentTransform().matrix() );
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
	if (vbo->needUpdate() || obj->hasChanged())
	{
		fillVertexBufferPoints(obj, vbo);
		
		vbo->setNeedUpdate(false);
        obj->setChanged(false);
		
		GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
		if (attColor >= 0) {
			glEnableVertexAttribArray(attColor);
			glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
								  sizeof(PointVtxStruct),
								  (GLvoid*)offsetof(PointVtxStruct, color));
		}
		GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
		if (attVtx >= 0) {
			glEnableVertexAttribArray(attVtx);
			glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
								  sizeof(PointVtxStruct),
								  (GLvoid*)offsetof(PointVtxStruct, v));
		}
		GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
		if (attNormal >= 0) {
			glEnableVertexAttribArray(attNormal);
			glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
								  sizeof(PointVtxStruct),
								  (GLvoid*)offsetof(PointVtxStruct, n));
		}
        GLint attTexCoord = mat->shaderProgram()->attributeLocation("glTexCoord");
		if (attTexCoord >= 0) {
			glEnableVertexAttribArray(attTexCoord);
			glVertexAttribPointer(attTexCoord, 2, GL_FLOAT, GL_FALSE,
								  sizeof(PointVtxStruct),
								  (GLvoid*)offsetof(PointVtxStruct, t));
		}
		THROW_IF_GLERROR("Failed to get attribute");
    }
	
	switch (state.renderMode) {
        case RenderMode::RM_Smooth:
		case RenderMode::RM_WireFrame:
		case RenderMode::RM_Points:
		{
			mat->shaderProgram()->useProgram();
			glPointSize(8.0f);
			GLsizei numVertices = vbo->getBufferSize() / sizeof(PointVtxStruct);
			glDrawArrays(GL_POINTS, 0, numVertices);
		}
			
		default:
			break;
	}
    
    
    vao->release();
	
}

VertexBuffer* PointCloudRenderable::getVBO(ISurface* mesh) const
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

VAO* PointCloudRenderable::getVAO(ISurface* mesh) const
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

void PointCloudRenderable::fillVertexBufferPoints(ISurface* mesh, VertexBuffer* vbo) const
{
    //std::cerr << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;
    
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;
    
    std::vector<PointVtxStruct> vtxData(numVertices); // Triangles
    
    size_t offset = 0;
    auto it = mesh->constVertexIterator();
    while(it.hasNext()) {
        auto v = static_cast<Vertex*>(it.next());
		vtxData[offset].v[0] = v->position().x();
		vtxData[offset].v[1] = v->position().y();
		vtxData[offset].v[2] = v->position().z();
		vtxData[offset].v[3] = 1.0f;
		vtxData[offset].n[0] = v->normal().x();
		vtxData[offset].n[1] = v->normal().y();
		vtxData[offset].n[2] = v->normal().z();
		vtxData[offset].n[3] = 0.0f;
        vtxData[offset].t[0] = v->texcoords().x();
        vtxData[offset].t[1] = v->texcoords().y();
		memcpy(vtxData[offset].color, v->color().data().data(), sizeof(vtxData[offset].color)) ;
        offset++;
    }
    // offset contains the number of vertices in the vtxData after being
    // processed.
    GLuint dataSize = static_cast<GLuint>(offset*sizeof(PointVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), dataSize);
    THROW_IF_GLERROR(__func__);
    
    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}

void PointCloudRenderable::render(RenderState & state) const
{
	renderObject(state);
}
