/***************************************************************************
 *   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Plastilina.h>

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/BufferObject.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Canvas.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <PlastilinaCore/opengl/FrameBufferObject.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

using namespace core::opencl;

struct VtxStruct
{
    GLfloat v[4];
    GLfloat color[4];
};

namespace core
{

class CanvasGL : public Canvas
{
  public:
    CanvasGL(int w, int h, bool renderToTexture = false);
    ~CanvasGL();

    void setup(int w, int h);

    virtual void setPenColor(const Color& c);
    virtual void setFillColor(const Color& c);
    virtual void begin();
    virtual void end();
    virtual void drawRectangle(float x, float y, float w, float h);
    virtual void drawEllipse(float x, float y, float w, float h) override;
    virtual void applyFilter();

    virtual std::shared_ptr<gl::Texture2D> colorTexture() { return color; }

    virtual std::shared_ptr<gl::Texture2D> depthTexture() { return depth; }

    bool isBound();

  protected:
    gl::FrameBufferObject          fbo;
    std::shared_ptr<gl::Texture2D> color;
    std::shared_ptr<gl::Texture2D> depth;

    GlslProgram           shaderProgram;
    VertexShader          vtxShader;
    FragmentShader        fragShader;
    VertexBuffer          vertexBuffer;
    gl::VertexArrayObject vertexArrayObject;

    Color                  penColor;
    Color                  fillColor;
    std::vector<VtxStruct> data;
};

class CanvasCL : public Canvas
{
  public:
    CanvasCL(int w, int h, bool renderToTexture = false);
    ~CanvasCL();

    void setup(int w, int h);

    virtual void setPenColor(const Color& c);
    virtual void setFillColor(const Color& c);
    virtual void begin();
    virtual void end();
    virtual void drawRectangle(float x, float y, float w, float h);
    virtual void drawEllipse(float x, float y, float w, float h) override;
    virtual void applyFilter();

    virtual std::shared_ptr<gl::Texture2D> colorTexture() { return color; }

    virtual std::shared_ptr<gl::Texture2D> depthTexture() { return depth; }

    bool isBound();

  protected:
    static bool          oclInitialized;
    static ::cl::Program program;
    static ::cl::Kernel  drawKernel;
    static ::cl::Kernel  drawCircleKernel;
    static ::cl::Kernel  inkDropKernel;
    static ::cl::Kernel  inkStepKernel;

    std::shared_ptr<gl::Texture2D> color;
    std::shared_ptr<gl::Texture2D> colorBack;
    std::shared_ptr<gl::Texture2D> depth;
    ::cl::ImageGL                  clColorImage;
    ::cl::ImageGL                  clColorBackImage;
    Color                          penColor;
    Color                          fillColor;
};

bool          CanvasCL::oclInitialized = false;
::cl::Program CanvasCL::program;
::cl::Kernel  CanvasCL::drawKernel;
::cl::Kernel  CanvasCL::drawCircleKernel;
::cl::Kernel  CanvasCL::inkDropKernel;
::cl::Kernel  CanvasCL::inkStepKernel;

std::unique_ptr<Canvas> core::Canvas::factory(CanvasBackEnd backend, int w,
                                              int h)
{
    switch (backend)
    {
    case CanvasBackEnd::OpenGL:
        return std::make_unique<CanvasGL>(w, h, false);
        break;
    case CanvasBackEnd::OpenCL:
        return std::make_unique<CanvasCL>(w, h, false);
        break;
    case CanvasBackEnd::GlTexture:
        return std::make_unique<CanvasGL>(w, h, true);
        break;
    case CanvasBackEnd::ClTexture:
        return std::make_unique<CanvasCL>(w, h, true);
        break;
    }
    return nullptr;
}

Canvas::Canvas(int w, int h) {}

Canvas::~Canvas() {}

CanvasGL::CanvasGL(int w, int h, bool renderToTexture)
    : Canvas(w, h)
{
    setup(w, h);
}

CanvasGL::~CanvasGL() { vertexBuffer.destroy(); }

void CanvasGL::setup(int w, int h)
{
    gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
    fbo.bind();
    color = std::make_shared<gl::Texture2D>();
    depth = std::make_shared<gl::Texture2D>();
    if (!color || !depth)
    {
        throw std::bad_alloc();
    }

    color->bind();
    color->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    color->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    color->texImage2D(0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                      nullptr);
    fbo.framebufferTexture(GL_COLOR_ATTACHMENT0, *color, 0);

    depth->bind();
    depth->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    depth->setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    depth->texImage2D(0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT,
                      GL_FLOAT, nullptr);
    fbo.framebufferTexture(GL_DEPTH_ATTACHMENT, *depth, 0);

    if (!fbo.isComplete())
    {
        TRACE(error) << "Failed to set framebuffer state";
    }

    ResourcesManager mgr;
    if (!vtxShader.compileStatus())
    {
        std::string path = mgr.findResourcePath("Canvas", "vs", "shaders");
        vtxShader.loadFromFile(path);
        if (!vtxShader.compile())
        {
            TRACE(error) << "vtxShader: Compilation failed";
            TRACE(error) << vtxShader.infoLog();
            throw core::GlException("Failed to compile vertex shader",
                                    glGetError());
        }
    }
    if (!fragShader.compileStatus())
    {
        std::string path = mgr.findResourcePath("Canvas", "fs", "shaders");
        fragShader.loadFromFile(path);
        if (!fragShader.compile())
        {
            TRACE(error) << "fragShader: Compilation failed";
            TRACE(error) << fragShader.infoLog();
            throw core::GlException("Failed to compile fragment shader",
                                    glGetError());
        }
    }

    shaderProgram.attachShader(&vtxShader);
    shaderProgram.attachShader(&fragShader);

    shaderProgram.bindAttributeLocation(0, "vertex");
    shaderProgram.bindAttributeLocation(1, "color");

    if (!shaderProgram.link())
    {
        TRACE(error) << "Link failed: \n" << shaderProgram.buildLog();
        throw core::GlException("Failed to link shader", glGetError());
    }
    THROW_IF_GLERROR("Failed here");

    vertexArrayObject.bind();
    data.resize(12);
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.setBufferData(data.data(), data.size() * sizeof(VtxStruct));

    GLint attColor = shaderProgram.attributeLocation("color");
    if (attColor >= 0)
    {
        glEnableVertexAttribArray(attColor);
        glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
                              sizeof(VtxStruct),
                              (GLvoid*)offsetof(VtxStruct, color));
    }
    GLint attVtx = shaderProgram.attributeLocation("vertex");
    if (attVtx >= 0)
    {
        glEnableVertexAttribArray(attVtx);
        glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                              sizeof(VtxStruct),
                              (GLvoid*)offsetof(VtxStruct, v));
    }
    vertexArrayObject.unbind();
}

void CanvasGL::setPenColor(const Color& c) { penColor = c; }

void CanvasGL::setFillColor(const Color& c) { fillColor = c; }

void CanvasGL::begin()
{
    fbo.bind();
    shaderProgram.useProgram();
    Camera camera;
    camera.setViewport(0, 0, 1280, 720);
    camera.setOrthoMatrix(0, 1280, 0, 720, -100, 100);
    GLint matId = shaderProgram.uniformLocation("glModelViewMatrix");
    Eigen::Affine3f t(camera.modelView());
    if (matId != -1)
        shaderProgram.setUniform(matId, t);

    matId = shaderProgram.uniformLocation("glProjectionMatrix");
    if (matId != -1)
        shaderProgram.setUniform(matId, camera.projection());
}

void CanvasGL::end()
{
    shaderProgram.useProgram();
    vertexArrayObject.bind();
    GLsizei numVertices = static_cast<GLsizei>(vertexBuffer.getBufferSize() / sizeof(VtxStruct));
    glDrawArrays(GL_TRIANGLES, 0, numVertices / 2);
    glDrawArrays(GL_LINE_LOOP, numVertices / 2, numVertices / 2);
    glFlush();
    fbo.unbind();
}

void CanvasGL::drawRectangle(float x, float y, float w, float h)
{
    data[0] = {{x, y, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[1] = {{x + w, y, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[2] = {{x + w, y + h, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[3] = {{x + w, y + h, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[4] = {{x, y + h, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[5] = {{x, y, 0, 1},
               {penColor.r(), penColor.g(), penColor.b(), penColor.a()}};
    data[6] = {{x, y, 0, 1},
               {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    data[7] = {{x + w, y, 0, 1},
               {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    data[8] = {{x + w, y + h, 0, 1},
               {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    data[9] = {{x + w, y + h, 0, 1},
               {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    data[10] = {{x, y + h, 0, 1},
                {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    data[11] = {{x, y, 0, 1},
                {fillColor.r(), fillColor.g(), fillColor.b(), fillColor.a()}};
    vertexBuffer.setBufferData(data.data(), data.size() * sizeof(VtxStruct));
}

void CanvasGL::drawEllipse(float x, float y, float w, float h) {}

void CanvasGL::applyFilter() {}

bool CanvasGL::isBound() { return false; }

CanvasCL::CanvasCL(int w, int h, bool renderToTexture)
    : Canvas(w, h)
{
    setup(w, h);
}

CanvasCL::~CanvasCL() {}

void CanvasCL::setup(int w, int h)
{
    gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);

    color     = std::make_shared<gl::Texture2D>();
    colorBack = std::make_shared<gl::Texture2D>();
    depth     = std::make_shared<gl::Texture2D>();
    if (!color || !colorBack || !depth)
    {
        throw std::bad_alloc();
    }

    color->bind();
    color->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    color->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    color->texImage2D(0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                      nullptr);

    colorBack->bind();
    colorBack->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    colorBack->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    colorBack->texImage2D(0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                          nullptr);

    depth->bind();
    depth->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    depth->setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    depth->texImage2D(0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT,
                      GL_FLOAT, nullptr);

    cl_int err = CL_SUCCESS;

    if (CanvasCL::oclInitialized)
    {
        return;
    }

    CLManager* oclManager = CLManager::instance();
    try
    {
        ResourcesManager mgr;
        clColorImage = ::cl::ImageGL(oclManager->context(), CL_MEM_READ_WRITE,
                                     GL_TEXTURE_2D, 0, color->oid(), 0);
        clColorBackImage =
            ::cl::ImageGL(oclManager->context(), CL_MEM_READ_WRITE,
                          GL_TEXTURE_2D, 0, colorBack->oid(), 0);

        std::string            path = mgr.findResourcePath("Canvas", "cl");
        std::string            kernelSource = loadFromFile(path);
        //::cl::Program::Sources source(
        //    1, std::make_pair(kernelSource.c_str(), kernelSource.length()));
        program = ::cl::Program(oclManager->context(), kernelSource);
        program.build("-I . -I ../share -I ../../share");

        drawKernel       = ::cl::Kernel(program, "drawRectangle", &err);
        drawCircleKernel = ::cl::Kernel(program, "drawEllipse", &err);
        inkDropKernel    = ::cl::Kernel(program, "ink_drop", &err);
        inkStepKernel    = ::cl::Kernel(program, "ink_step", &err);
        // TRACE(trace)
        //    << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: "
        //    << drawKernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(NULL);
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
    CanvasCL::oclInitialized = true;
    return;
}

void CanvasCL::setPenColor(const Color& c) { penColor = c; }

void CanvasCL::setFillColor(const Color& c) { fillColor = c; }

void CanvasCL::begin() {}

void CanvasCL::end() {}

void CanvasCL::drawRectangle(float x, float y, float w, float h)
{
    try
    {
        CLManager* clmgr = CLManager::instance();

        std::vector<::cl::Memory> list;
        list.push_back(clColorImage);
        list.push_back(clColorBackImage);
        clmgr->commandQueue().enqueueAcquireGLObjects(&list);
        drawKernel.setArg(0, clColorBackImage);
        cl_float4 color = core::utils::convert_to<cl_float4>(penColor.data());
        drawKernel.setArg(1, color);
        color = core::utils::convert_to<cl_float4>(fillColor.data());
        drawKernel.setArg(2, color);
        drawKernel.setArg(3, uint32_t(x));
        drawKernel.setArg(4, uint32_t(y));
        drawKernel.setArg(5, uint32_t(w));
        drawKernel.setArg(6, uint32_t(h));
        clmgr->commandQueue().enqueueNDRangeKernel(
            drawKernel, ::cl::NullRange, ::cl::NDRange(1280, 720));
        clmgr->commandQueue().enqueueReleaseGLObjects(&list);
        clmgr->commandQueue().flush();
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << errorToString(e.err()) << "): " << e.what();
    }
}

void CanvasCL::drawEllipse(float x, float y, float w, float h)
{
    try
    {
        CLManager* clmgr = CLManager::instance();

        std::vector<::cl::Memory> list;
        list.push_back(clColorImage);
        list.push_back(clColorBackImage);
        clmgr->commandQueue().enqueueAcquireGLObjects(&list);
        drawCircleKernel.setArg(0, clColorBackImage);
        cl_float4 color = core::utils::convert_to<cl_float4>(penColor.data());
        drawCircleKernel.setArg(1, color);
        color = core::utils::convert_to<cl_float4>(fillColor.data());
        drawCircleKernel.setArg(2, color);
        drawCircleKernel.setArg(3, uint32_t(x));
        drawCircleKernel.setArg(4, uint32_t(y));
        drawCircleKernel.setArg(5, uint32_t(w));
        drawCircleKernel.setArg(6, uint32_t(h));
        clmgr->commandQueue().enqueueNDRangeKernel(
            drawCircleKernel, ::cl::NullRange, ::cl::NDRange(1280, 720));
        clmgr->commandQueue().enqueueReleaseGLObjects(&list);
        clmgr->commandQueue().flush();
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << errorToString(e.err()) << "): " << e.what();
    }
}

void CanvasCL::applyFilter()
{
    try
    {
        CLManager* clmgr = CLManager::instance();

        std::vector<::cl::Memory> list;
        list.push_back(clColorImage);
        list.push_back(clColorBackImage);
        clmgr->commandQueue().enqueueAcquireGLObjects(&list);
        inkStepKernel.setArg(0, clColorImage);
        inkStepKernel.setArg(1, clColorBackImage);
        inkStepKernel.setArg(2, nullptr);
        inkStepKernel.setArg(3, nullptr);
        clmgr->commandQueue().enqueueNDRangeKernel(
            inkStepKernel, ::cl::NullRange, ::cl::NDRange(1280, 720));
        std::array<size_t,3> region;
        region[0] = 1280;
        region[1] = 720;
        region[2] = 1;
        clmgr->commandQueue().enqueueCopyImage(clColorImage, clColorBackImage,
                                               std::array<size_t,3>(),
                                               std::array<size_t,3>(), region);
        clmgr->commandQueue().enqueueReleaseGLObjects(&list);
        clmgr->commandQueue().flush();
    }
    catch (::cl::Error& e)
    {
        TRACE(error) << "OpenCL exception:" << e.err() << " ("
                     << errorToString(e.err()) << "): " << e.what();
    }
}

bool CanvasCL::isBound() { return false; }
};
