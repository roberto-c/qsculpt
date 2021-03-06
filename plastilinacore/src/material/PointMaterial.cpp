//
//  PhongMaterial.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/material/PointMaterial.h>

#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>

#define VERTEX_SHADER_PATH "/shaders/Point.vs"
#define FRAGMENT_SHADER_PATH "/shaders/Point.fs"

struct PointMaterial::Impl
{
    VertexShader   vtxShader;
    FragmentShader fragShader;

    CameraNode::shared_ptr             camera;
    std::vector<LightNode::shared_ptr> lights;

    bool initialized;

    Impl()
        : initialized(false)
    {
    }
};
PointMaterial::PointMaterial()
    : Material()
    , d(new Impl)
{
}

PointMaterial::~PointMaterial() {}

void PointMaterial::load()
{
    if (d->initialized)
    {
        return;
    }

    ResourcesManager mgr;
    if (!d->vtxShader.compileStatus())
    {
        std::string path = mgr.findResourcePath("Point", "vs", "shaders");
        d->vtxShader.loadFromFile(path);
        if (!d->vtxShader.compile())
        {
            std::cerr << "vtxShader: Compilation failed" << std::endl;
            std::cerr << d->vtxShader.infoLog() << std::endl;
        }
    }
    if (!d->fragShader.compileStatus())
    {
        std::string path = mgr.findResourcePath("Point", "fs", "shaders");
        d->fragShader.loadFromFile(path);
        if (!d->fragShader.compile())
        {
            std::cerr << "fragShader: Compilation failed" << std::endl;
            std::cerr << d->fragShader.infoLog() << std::endl;
        }
    }

    shaderProgram()->attachShader(&d->vtxShader);
    shaderProgram()->attachShader(&d->fragShader);

    shaderProgram()->bindAttributeLocation(0, "glVertex");
    shaderProgram()->bindAttributeLocation(1, "glColor");

    if (!shaderProgram()->link())
    {
        std::cerr << "Link failed: \n"
                  << shaderProgram()->buildLog() << std::endl;
    }

    GLint n = shaderProgram()->fragDataLocation("glFragColor");
    std::cerr << "FragLocation: " << n << std::endl;

    std::string name;
    GLenum      type   = 0;
    GLint       size   = 0;
    GLint       actAtt = shaderProgram()->activeAttributes();
    for (int i = 0; i < actAtt; ++i)
    {
        shaderProgram()->activeAttrib(i, &name, &type, &size);
        std::cerr << "Attribute[" << i << "] = " << name.c_str() << " ("
                  << std::hex << type << std::dec << "[" << size << "])\n";
    }

    d->initialized = true;
}

void PointMaterial::unload() {}

void PointMaterial::setup(const std::shared_ptr<SceneNode>& node)
{
    setup(std::dynamic_pointer_cast<const SceneNode>(node));
}

void PointMaterial::setup(const std::shared_ptr<const SceneNode>& node)
{
    Eigen::Vector4f camPos(0, 0, 0, 1);
    Eigen::Vector4f lightPos(4, 0, 0, 1);

    THROW_IF_GLERROR("Failed to set params");
}
