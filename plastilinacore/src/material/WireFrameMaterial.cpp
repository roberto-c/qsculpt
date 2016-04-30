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
#include <PlastilinaCore/material/WireFrameMaterial.h>

#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/ResourcesManager.h>


struct WireFrameMaterial::Impl
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Color           outline;
    Color           fill;

    VertexShader    vtxShader;
    FragmentShader  fragShader;

    CameraNode::shared_ptr              camera;
    std::vector<LightNode::shared_ptr>  lights;

    bool            initialized;

    Impl() : initialized(false) {}
};

WireFrameMaterial::WireFrameMaterial()
    : Material(),
    d(new Impl)
{

}

WireFrameMaterial::~WireFrameMaterial()
{

}

void WireFrameMaterial::load()
{
    if (d->initialized) {
        return;
    }

    ResourcesManager mgr;
    if (!d->vtxShader.compileStatus()) {
        std::string path = mgr.findResourcePath("WireFrame", "vs", "shaders");
        d->vtxShader.loadFromFile(path);
        if (!d->vtxShader.compile()) {
            std::cerr << "vtxShader: Compilation failed" << std::endl;
            std::cerr << d->vtxShader.infoLog() << std::endl;
            throw core::GlException("Failed to compile vertex shader", glGetError());
        }
    }
    if (!d->fragShader.compileStatus()) {
        std::string path = mgr.findResourcePath("WireFrame", "fs", "shaders");
        d->fragShader.loadFromFile(path);
        if (!d->fragShader.compile()) {
            std::cerr << "fragShader: Compilation failed" << std::endl;
            std::cerr << d->fragShader.infoLog() << std::endl;
            throw core::GlException("Failed to compile fragment shader", glGetError());
        }
    }

    shaderProgram()->attachShader(&d->vtxShader);
    shaderProgram()->attachShader(&d->fragShader);

    shaderProgram()->bindAttributeLocation(0, "glVertex");
    shaderProgram()->bindAttributeLocation(1, "glNormal");
    shaderProgram()->bindAttributeLocation(2, "glColor");

    if (!shaderProgram()->link()) {
        std::cerr << "Link failed: \n" << shaderProgram()->buildLog() << std::endl;
    }

    GLint n = shaderProgram()->fragDataLocation("glFragColor");
    std::cerr << "FragLocation: " << n << std::endl;

    std::string name;
    GLenum type = 0;
    GLint size = 0;
    GLint actAtt = shaderProgram()->activeAttributes();
    for (int i = 0; i < actAtt; ++i) {
        shaderProgram()->activeAttrib(i, &name, &type, &size);
        std::cerr << "Attribute[" << i << "] = " << name.c_str()
            << " (" << std::hex << type << std::dec << "[" << size << "])\n";
    }

    // Default material values
    setOutlineColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
    setFillColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    d->initialized = true;
}

void WireFrameMaterial::unload()
{

}


void WireFrameMaterial::setup(const std::shared_ptr<SceneNode> & doc)
{
    setup(std::dynamic_pointer_cast<const SceneNode>(doc));
}

void WireFrameMaterial::setup(const std::shared_ptr<const SceneNode> & doc)
{
    Eigen::Vector4f camPos(0, 0, 0, 1);
    Eigen::Vector4f lightPos(4, 0, 0, 1);

    GLint outline = shaderProgram()->uniformLocation("outlineColor");
    GLint fill = shaderProgram()->uniformLocation("fillColor");
    if (outline != -1) shaderProgram()->setUniform(outline, d->outline.data());
    if (fill != -1) shaderProgram()->setUniform(fill, d->fill.data());
    
    THROW_IF_GLERROR("Failed to set params");
}

Color WireFrameMaterial::outlineColor() const
{
    return d->outline;
}

void WireFrameMaterial::setOutlineColor(const Color & c)
{
    d->outline = c;
}

Color WireFrameMaterial::fillColor() const
{
    return d->fill;
}

void WireFrameMaterial::setFillColor(const Color & c)
{
    d->fill = c;
}

