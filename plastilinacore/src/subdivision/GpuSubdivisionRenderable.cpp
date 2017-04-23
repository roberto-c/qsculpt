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
#include <PlastilinaCore/subdivision/GpuSubdivisionRenderable.h>
#include <PlastilinaCore/subdivision/GpuSubdivision_private.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/RenderState.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SmoothRenderer.h>

#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

#define BO_POOL_NAME "SmoothRendererPool"

namespace core
{
using core::gpusubdivision::Vertex;
using core::gpusubdivision::Face;
using core::gpusubdivision::Edge;

struct GpuSubdivisionRenderable::Impl
{
    const core::GpuSubdivision* surface;

    static bool          oclInitialized;
    static ::cl::Program program;
    static ::cl::Kernel  krnGenerateMesh;

    Impl() {}

    bool initializeOcl();

    void renderObject(RenderState& state) const;

    VertexBuffer* getVBO(ISurface* mesh) const;

    VAO* getVAO(ISurface* mesh) const;

    void build_mesh(const core::GpuSubdivision& surface) const;
};

bool          GpuSubdivisionRenderable::Impl::oclInitialized = false;
::cl::Program GpuSubdivisionRenderable::Impl::program;
::cl::Kernel  GpuSubdivisionRenderable::Impl::krnGenerateMesh;

GpuSubdivisionRenderable::GpuSubdivisionRenderable(
    const core::GpuSubdivision* surface)
    : _d(new Impl())
{
    _d->surface = surface;
    _d->initializeOcl();
}

GpuSubdivisionRenderable::~GpuSubdivisionRenderable() {}

void GpuSubdivisionRenderable::render(RenderState& state) const
{
    _d->renderObject(state);
}

bool GpuSubdivisionRenderable::Impl::initializeOcl()
{
    cl_int err = CL_INVALID_OPERATION;

    if (GpuSubdivisionRenderable::Impl::oclInitialized)
    {
        return true;
    }

    CLManager* oclManager = CLManager::instance();
    try
    {
        ResourcesManager mgr;
        std::string      path = mgr.findResourcePath("Subdivision", "cl");
        std::string      kernelSource = core::cl::loadFromFile(path);
        program = ::cl::Program(oclManager->context(), kernelSource);
		auto options = "-I . -I " + path.substr(0, path.find("Subdivision"));
        program.build(options.c_str());

        krnGenerateMesh = ::cl::Kernel(program, "build_mesh");
        //TRACE(trace)
        //    << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: "
        //    << krnGenerateMesh.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(
        //           NULL);
        GpuSubdivisionRenderable::Impl::oclInitialized = true;
        return true;
    }
    catch (::cl::Error err)
    {
        TRACE(error) << "ERROR: " << err.what() << "(" << err.err() << ")";
        if (err.err() == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string buildlog;
            program.getBuildInfo(oclManager->devices()[0],
                                 CL_PROGRAM_BUILD_LOG, &buildlog);
            TRACE(error) << "Build log: " << buildlog << std::endl;
        }
    }
    catch (const std::runtime_error& e)
    {
        TRACE(error) << "ERROR: " << e.what();
    }

    return false;
}

void GpuSubdivisionRenderable::Impl::renderObject(RenderState& state) const
{
    GpuSubdivision*           obj = NULL;
    std::shared_ptr<Material> mat;

    auto node = state.currentNode;
    if (!node)
    {
        return;
    }
    auto snode = std::dynamic_pointer_cast<const SurfaceNode>(node);
    if (!snode)
    {
        std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
        return;
    }

    if (snode->material() && snode->material()->shaderProgram())
    {
        snode->material()->shaderProgram()->useProgram();
        snode->material()->setup(state.root);
    }

    GlslProgram* prog = GlslProgram::currentProgram();
    if (prog->programID() > 0)
    {
        auto            cameraNode = state.camera;
        Camera*         camera     = cameraNode->camera().get();
        GLint           matId = prog->uniformLocation("glModelViewMatrix");
        Eigen::Affine3f t(camera->modelView());
        if (matId != -1)
            prog->setUniform(matId, t);

        matId       = prog->uniformLocation("objectTransform");
        auto ntrans = node->transform() * node->parentTransform();
        if (matId != -1)
            prog->setUniform(matId, ntrans);

        matId = prog->uniformLocation("glProjectionMatrix");
        if (matId != -1)
            prog->setUniform(matId, camera->projection());

        Eigen::Vector3f p      = t.translation();
        Eigen::Vector4f camPos = Eigen::Vector4f(p[0], p[1], p[2], 1.0f);
        matId                  = prog->uniformLocation("eyePosition");
        if (matId != -1)
            prog->setUniform(matId, camPos);
    }

    obj = static_cast<GpuSubdivision*>(snode->surface());
    mat = snode->material();
    if (obj == NULL || mat == NULL)
        return;

    BufferObject* bo = core::cl::get_glbuffer_backing_store(
        *(this->surface->_d->_triangleOutput));
    VertexBuffer* vbo = static_cast<VertexBuffer*>(bo);
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
        build_mesh(*obj);
        vbo->setNeedUpdate(false);

        GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
        if (attColor >= 0)
        {
            glEnableVertexAttribArray(attColor);
            glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(GLVertexData),
                                  (GLvoid*)offsetof(GLVertexData, c));
        }
        GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
        if (attVtx >= 0)
        {
            glEnableVertexAttribArray(attVtx);
            glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(GLVertexData),
                                  (GLvoid*)offsetof(GLVertexData, p));
        }
        GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
        if (attNormal >= 0)
        {
            glEnableVertexAttribArray(attNormal);
            glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(GLVertexData),
                                  (GLvoid*)offsetof(GLVertexData, n));
        }
        GLint attTexCoord =
            mat->shaderProgram()->attributeLocation("glTexCoord");
        if (attTexCoord >= 0)
        {
            glEnableVertexAttribArray(attTexCoord);
            glVertexAttribPointer(attTexCoord, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(GLVertexData),
                                  (GLvoid*)offsetof(GLVertexData, t));
        }
        THROW_IF_GLERROR("Failed to get attribute");
    }

    switch (state.renderMode)
    {
    case RenderMode::RM_Smooth:
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mat->shaderProgram()->useProgram();
        GLsizei numVertices = vbo->getBufferSize() / sizeof(GLVertexData);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
    break;

    case RenderMode::RM_WireFrame:
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mat->shaderProgram()->useProgram();
        GLsizei numVertices = vbo->getBufferSize() / sizeof(GLVertexData);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
    break;
    case RenderMode::RM_Points:
    {
        mat->shaderProgram()->useProgram();
        glPointSize(3.0f);
        GLsizei numVertices = vbo->getBufferSize() / sizeof(GLVertexData);
        glDrawArrays(GL_POINTS, 0, numVertices);
    }

    default:
        break;
    }

    vao->unbind();
}

VertexBuffer* GpuSubdivisionRenderable::Impl::getVBO(ISurface* mesh) const
{
    VertexBuffer* vbo = NULL;
    vbo               = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

VAO* GpuSubdivisionRenderable::Impl::getVAO(ISurface* mesh) const
{
    VAO* vao = NULL;
    vao      = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
    if (vao == NULL)
    {
        vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
    }
    return vao;
}

void GpuSubdivisionRenderable::Impl::build_mesh(
    const core::GpuSubdivision& surface) const
{
    try
    {
        CLManager* clmgr = CLManager::instance();

        std::vector<::cl::Memory> list;
        ::cl::Memory              mem(
            core::cl::get_mem_backing_store(*(surface._d->_triangleOutput)), true);
        list.push_back(mem);
        clmgr->commandQueue().enqueueAcquireGLObjects(&list);
        this->krnGenerateMesh.setArg(
            0, core::cl::get_mem_backing_store(*(surface._d->_vertices)));
        this->krnGenerateMesh.setArg(1, surface._d->_vertices->size());
        this->krnGenerateMesh.setArg(
            2, core::cl::get_mem_backing_store(*(surface._d->_edges)));
        this->krnGenerateMesh.setArg(3, surface._d->_edges->size());
        this->krnGenerateMesh.setArg(
            4, core::cl::get_mem_backing_store(*(surface._d->_faces)));
        this->krnGenerateMesh.setArg(5, surface._d->_faces->size());
        this->krnGenerateMesh.setArg(6, mem);
        this->krnGenerateMesh.setArg(7, surface._d->_triangleOutput->size());
        clmgr->commandQueue().enqueueNDRangeKernel(
            this->krnGenerateMesh, ::cl::NullRange,
            ::cl::NDRange(surface._d->_faces->size()));
        clmgr->commandQueue().enqueueReleaseGLObjects(&list);
        clmgr->commandQueue().flush();
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << core::cl::errorToString(e.err()) << "): " << e.what();
    }
}

}; // namspace core
