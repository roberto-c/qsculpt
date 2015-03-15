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
#include <PlastilinaCore/subdivision/SubdivisionRenderable.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/SmoothRenderer.h>
#include <PlastilinaCore/Scene.h>

#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "SmoothRendererPool"

struct SmoothVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat color[4];
    GLfloat t[2];
}; 


SubdivisionRenderable::SubdivisionRenderable(const Subdivision * surface)
{
	std::cerr << "SmoothRenderer constructor" << std::endl;
}

SubdivisionRenderable::~SubdivisionRenderable()
{
	std::cerr << "SmoothRenderer destructor" << std::endl;
	BOManager::getInstance()->destroyPool(BO_POOL_NAME);
}

void SubdivisionRenderable::renderObject(const RenderState * state) const
{
	ISurface * obj = NULL;
	std::shared_ptr<Material> mat;
	
	const SceneNode * node = state->currentNode;
	if (!node) {
		return;
	}
	const SurfaceNode * snode = dynamic_cast<const SurfaceNode*>(node);
	if (!snode) {
		std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
		return;
	}
	
	if (snode->material() && snode->material()->shaderProgram()) {
		snode->material()->shaderProgram()->useProgram();
		snode->material()->setup(state->root->shared_from_this());
	}
	
	GlslProgram * prog = GlslProgram::currentProgram();
	if (prog->programID() > 0) {
		CameraNode * cameraNode = state->camera;
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
		if (state->renderMode == RM_Points) {
			fillVertexBufferPoints(obj, vbo);
		} else {
			fillVertexBuffer(obj, vbo);
		}
		
		vbo->setNeedUpdate(false);
		
		GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
		if (attColor >= 0) {
			glEnableVertexAttribArray(attColor);
			glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
								  sizeof(SmoothVtxStruct),
								  (GLvoid*)offsetof(SmoothVtxStruct, color));
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
	
	switch (state->renderMode) {
		case RM_Smooth:
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			mat->shaderProgram()->useProgram();
			GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		}
			break;
			
		case RM_WireFrame:
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mat->shaderProgram()->useProgram();
			GLsizei numVertices = vbo->getBufferSize() / sizeof(SmoothVtxStruct);
			glDrawArrays(GL_TRIANGLES, 0, numVertices);
		}
			break;
		case RM_Points:
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

VertexBuffer* SubdivisionRenderable::getVBO(ISurface* mesh) const
{
	VertexBuffer* vbo = NULL;
	vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
	if (vbo == NULL)
	{
		vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
	}
	return vbo;
}

VAO* SubdivisionRenderable::getVAO(ISurface* mesh) const
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

void SubdivisionRenderable::fillVertexBufferPoints(ISurface* mesh, VertexBuffer* vbo) const
{
    //std::cerr << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;
    
    size_t numVertices = mesh->numVertices();
    if (numVertices == 0)
        return;
    
    std::vector<SmoothVtxStruct> vtxData(numVertices); // Triangles
    
    size_t offset = 0;
    auto it = mesh->constVertexIterator();
    while(it.hasNext()) {
        auto v = it.next();
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
    GLuint dataSize = static_cast<GLuint>(offset*sizeof(SmoothVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), dataSize);
    THROW_IF_GLERROR(__func__);
    
    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}


void SubdivisionRenderable::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo) const
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
        numFaces += it.next()->numVertices() - 2;
    }
    
    size_t numVertices = numFaces*3;
    std::vector<SmoothVtxStruct> vtxData(numVertices); // Triangles
    
    size_t offset = 0;
    it = mesh->constFaceIterator();
    while(it.hasNext()) {
        auto f = it.next();
        processPolygon(*f, vtxData, offset);
    }
    // offset contains the number of vertices in the vtxData after being
    // processed.
    GLuint dataSize = static_cast<GLuint>(offset*sizeof(SmoothVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), dataSize);
    THROW_IF_GLERROR(__func__);
    
    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}

bool SubdivisionRenderable::processPolygon(const Face & f,
										  std::vector<SmoothVtxStruct> & vtxData,
										  size_t & offset) const
{
    size_t nVtx = f.numVertices();
    if (nVtx < 3) {
        std::cerr << "Incomplete polygon. A polygon should have at least 3 vertices" << std::endl;
        return false;
    }
    //GLfloat * color = f.flags() && FF_Selected ? g_selectedColor : g_normalColor;
    
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

bool SubdivisionRenderable::processTriangle(const Vertex & v1,
										   const Vertex & v2,
										   const Vertex & v3,
										   std::vector<SmoothVtxStruct> & vtxData,
										   size_t & offset) const
{
    vtxData[offset].v[0] = v1.position().x();
    vtxData[offset].v[1] = v1.position().y();
    vtxData[offset].v[2] = v1.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v1.normal().x();
    vtxData[offset].n[1] = v1.normal().y();
    vtxData[offset].n[2] = v1.normal().z();
    vtxData[offset].n[3] = 0;
    vtxData[offset].t[0] = v1.texcoords().x();
    vtxData[offset].t[1] = v1.texcoords().y();
    memcpy(vtxData[offset].color, v1.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v2.position().x();
    vtxData[offset].v[1] = v2.position().y();
    vtxData[offset].v[2] = v2.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v2.normal().x();
    vtxData[offset].n[1] = v2.normal().y();
    vtxData[offset].n[2] = v2.normal().z();
    vtxData[offset].n[3] = 0;
    vtxData[offset].t[0] = v2.texcoords().x();
    vtxData[offset].t[1] = v2.texcoords().y();
    memcpy(vtxData[offset].color, v2.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    vtxData[offset].v[0] = v3.position().x();
    vtxData[offset].v[1] = v3.position().y();
    vtxData[offset].v[2] = v3.position().z();
    vtxData[offset].v[3] = 1;
    vtxData[offset].n[0] = v3.normal().x();
    vtxData[offset].n[1] = v3.normal().y();
    vtxData[offset].n[2] = v3.normal().z();
    vtxData[offset].n[3] = 0;
    vtxData[offset].t[0] = v3.texcoords().x();
    vtxData[offset].t[1] = v3.texcoords().y();
    memcpy(vtxData[offset].color, v3.color().data().data(), sizeof(vtxData[offset].color)) ;
    offset++;
    
    return true;
}


void SubdivisionRenderable::render(const RenderState * state) const
{
	renderObject(state);
}
