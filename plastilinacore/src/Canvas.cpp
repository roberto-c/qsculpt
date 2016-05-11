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
#include <PlastilinaCore/opengl/FrameBufferObject.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>
#include <PlastilinaCore/ResourcesManager.h>

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
    CanvasGL(int w, int h);
    ~CanvasGL();

    void setup(int w, int h);


    virtual void setPenColor(const Color & c);
    virtual void setFillColor(const Color & c);
    virtual void begin();
    virtual void end();
    virtual void drawRectangle(float x, float y, float w, float h);

    virtual std::shared_ptr<gl::Texture2D> colorTexture() {
        return color;
    }

    virtual std::shared_ptr<gl::Texture2D> depthTexture()
    {
        return depth;
    }

    bool isBound();

protected:
    gl::FrameBufferObject           fbo;
    std::shared_ptr<gl::Texture2D>  color;
    std::shared_ptr<gl::Texture2D>  depth;

    GlslProgram     shaderProgram;
    VertexShader    vtxShader;
    FragmentShader  fragShader;
    VertexBuffer    vertexBuffer;
    gl::VertexArrayObject vertexArrayObject;

    Color   penColor;
    Color   fillColor;
    std::vector<VtxStruct> data;
};

std::unique_ptr<Canvas> core::Canvas::factory(CanvasBackEnd backend, int w, int h)
{
    return std::make_unique<CanvasGL>(w, h);
}

Canvas::Canvas(int w, int h)
{
}

Canvas::~Canvas()
{
}

CanvasGL::CanvasGL(int w, int h)
    : Canvas(w, h)
{
    setup(w, h);
}

CanvasGL::~CanvasGL()
{
    vertexBuffer.destroy();
    
}

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
    color->texImage2D(0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    fbo.framebufferTexture(GL_COLOR_ATTACHMENT0, *color, 0);

    depth->bind();
    depth->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    depth->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    depth->setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    depth->setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    depth->texImage2D(0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    fbo.framebufferTexture(GL_DEPTH_ATTACHMENT, *depth, 0);

    if (!fbo.isComplete())
    {
        TRACE(error) << "Failed to set framebuffer state";
    }

    ResourcesManager mgr;
    if (!vtxShader.compileStatus()) {
        std::string path = mgr.findResourcePath("Canvas", "vs", "shaders");
        vtxShader.loadFromFile(path);
        if (!vtxShader.compile()) {
            TRACE(error) << "vtxShader: Compilation failed";
            TRACE(error) << vtxShader.infoLog();
            throw core::GlException("Failed to compile vertex shader", glGetError());
        }
    }
    if (!fragShader.compileStatus()) {
        std::string path = mgr.findResourcePath("Canvas", "fs", "shaders");
        fragShader.loadFromFile(path);
        if (!fragShader.compile()) {
            TRACE(error) << "fragShader: Compilation failed";
            TRACE(error) << fragShader.infoLog();
            throw core::GlException("Failed to compile fragment shader", glGetError());
        }
    }

    shaderProgram.attachShader(&vtxShader);
    shaderProgram.attachShader(&fragShader);

    shaderProgram.bindAttributeLocation(0, "vertex");
    shaderProgram.bindAttributeLocation(1, "color");

    if (!shaderProgram.link()) {
        TRACE(error) << "Link failed: \n" << shaderProgram.buildLog();
        throw core::GlException("Failed to link shader", glGetError());
    }
    THROW_IF_GLERROR("Failed here");
    
    vertexArrayObject.bind();
    data.resize(6);
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.setBufferData(data.data(), data.size() * sizeof(VtxStruct));

    GLint attColor = shaderProgram.attributeLocation("color");
    if (attColor >= 0) {
        glEnableVertexAttribArray(attColor);
        glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
            sizeof(VtxStruct),
            (GLvoid*)offsetof(VtxStruct, color));
    }
    GLint attVtx = shaderProgram.attributeLocation("vertex");
    if (attVtx >= 0) {
        glEnableVertexAttribArray(attVtx);
        glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
            sizeof(VtxStruct),
            (GLvoid*)offsetof(VtxStruct, v));
    }
    vertexArrayObject.unbind();
}


void CanvasGL::setPenColor(const Color & c)
{
    penColor = c;
}

void CanvasGL::setFillColor(const Color & c)
{
    fillColor = c;
}

void CanvasGL::begin()
{
    fbo.bind();
    shaderProgram.useProgram();
    Camera camera;
    camera.setViewport(0, 0, 1280, 720);
    camera.setOrthoMatrix(0, 1280, 0, 720, -100, 100);
    GLint matId = shaderProgram.uniformLocation("glModelViewMatrix");
    Eigen::Affine3f t(camera.modelView());
    if (matId != -1) shaderProgram.setUniform(matId, t);

    matId = shaderProgram.uniformLocation("glProjectionMatrix");
    if (matId != -1) shaderProgram.setUniform(matId, camera.projection());
}

void CanvasGL::end()
{
    shaderProgram.useProgram();
    vertexArrayObject.bind();
    GLsizei numVertices = vertexBuffer.getBufferSize() / sizeof(VtxStruct);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glFlush();
    fbo.unbind();
}

void CanvasGL::drawRectangle(float x, float y, float w, float h)
{
    data[0] = { { x, y, 0, 1 }, { penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    data[1] = { { x + w, y, 0, 1 },{ penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    data[2] = { { x + w, y + h, 0, 1 },{ penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    data[3] = { { x + w, y + h, 0, 1 },{ penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    data[4] = { { x , y + h, 0, 1 },{ penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    data[5] = { { x , y, 0, 1 },{ penColor.r(), penColor.g(), penColor.b(), penColor.a() } };
    vertexBuffer.setBufferData(data.data(), data.size() * sizeof(VtxStruct));
}

bool CanvasGL::isBound()
{
    return false;
}

};
