//
//  PhongMaterial.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/material/PhongMaterial.h>

#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/ResourcesManager.h>

struct PhongMaterial::Impl
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Color           specular;
    Color           diffuse;
    Color           ambient;
    float           exponent;
    
    VertexShader    vtxShader;
    FragmentShader  fragShader;

    gl::Texture2D::shared_ptr           diffuseTex;
    
    CameraNode::shared_ptr              camera;
    std::vector<LightNode::shared_ptr>  lights;
    
    bool            initialized;
    
    Impl() : initialized(false){}
};

PhongMaterial::PhongMaterial()
: Material(),
d(new Impl)
{
    
}

PhongMaterial::~PhongMaterial()
{
    
}

void PhongMaterial::load()
{
    if (d->initialized) {
        return;
    }
	
	ResourcesManager mgr;
    if (!d->vtxShader.compileStatus()) {
		std::string path = mgr.findResourcePath("Phong", "vs", "shaders");
        d->vtxShader.loadFromFile(path);
        if (!d->vtxShader.compile()){
            TRACE(error) << "vtxShader: Compilation failed";
            TRACE(error) << d->vtxShader.infoLog();
            throw core::GlException("Failed to compile vertex shader", glGetError());
        }
    }
    if (!d->fragShader.compileStatus()){
		std::string path = mgr.findResourcePath("Phong", "fs", "shaders");
        d->fragShader.loadFromFile(path);
        if (!d->fragShader.compile()){
            TRACE(error) << "fragShader: Compilation failed";
            TRACE(error) << d->fragShader.infoLog();
            throw core::GlException("Failed to compile fragment shader", glGetError());
        }
    }
    
    shaderProgram()->attachShader(&d->vtxShader);
    shaderProgram()->attachShader(&d->fragShader);
    
    shaderProgram()->bindAttributeLocation(0, "glVertex");
    shaderProgram()->bindAttributeLocation(1, "glNormal");
    shaderProgram()->bindAttributeLocation(2, "glColor");
    
    if (!shaderProgram()->link()) {
        TRACE(error) << "Link failed: \n" << shaderProgram()->buildLog();
        throw core::GlException("Failed to link shader", glGetError());
    }
    
    GLint n = shaderProgram()->fragDataLocation("glFragColor");
    TRACE(trace) << "FragLocation: " << n;
    
    std::string name;
    GLenum type = 0;
    GLint size = 0;
    GLint actAtt = shaderProgram()->activeAttributes();
    for (int i = 0; i < actAtt; ++i) {
        shaderProgram()->activeAttrib(i, &name, &type, &size);
        TRACE(trace) << "Attribute[" << i << "] = " << name.c_str()
            << " ("<< std::hex << type << std::dec << "[" << size <<"])";
    }
    
    // Default material values
    setDiffuse (Color(0.8f, 0.8f, 0.8f, 1.0f));
    setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
    setAmbient (Color(0.1f, 0.1f, 0.1f, 1.0f));
    setExponent(200);
    
    d->initialized = true;
}

void PhongMaterial::unload()
{
    
}

void PhongMaterial::setup(const std::shared_ptr<SceneNode> & doc)
{
    setup(std::dynamic_pointer_cast<const SceneNode>(doc));
}

void PhongMaterial::setup(const std::shared_ptr<const SceneNode> & doc)
{
    Eigen::Vector4f camPos(0,0,0,1);
    Eigen::Vector4f lightPos(4,0,0,1);
    
    GLint diff = shaderProgram()->uniformLocation("diffuseColor");
    GLint spec = shaderProgram()->uniformLocation("specularColor");
    GLint amb = shaderProgram()->uniformLocation("ambientColor");
    GLint exp = shaderProgram()->uniformLocation("exponent");
    GLint lightPosId = shaderProgram()->uniformLocation("lightPosition");
    GLint textureSamplerId = shaderProgram()->uniformLocation("textureSampler");
    
    if (diff != -1) shaderProgram()->setUniform(diff, d->diffuse.data());
    if (spec != -1) shaderProgram()->setUniform(spec, d->specular.data());
    if (amb != -1) shaderProgram()->setUniform(amb, d->ambient.data());
    if (exp != -1) shaderProgram()->setUniform(exp, d->exponent);
    if (lightPosId != -1) shaderProgram()->setUniform(lightPosId, lightPos);
    if (textureSamplerId != -1) shaderProgram()->setUniform(textureSamplerId, 0);

    if (d->diffuseTex) {
        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        d->diffuseTex->bind();
    }

    THROW_IF_GLERROR("Failed to set params");
}



float PhongMaterial::exponent() const
{
    return d->exponent;
}

void PhongMaterial::setExponent(float exp)
{
    d->exponent = exp;
}

Color PhongMaterial::specular() const
{
    return d->specular;
}

void PhongMaterial::setSpecular(const Color & c)
{
    d->specular = c;
}

Color PhongMaterial::diffuse() const
{
    return d->diffuse;
}

void PhongMaterial::setDiffuse(const Color & c)
{
    d->diffuse = c;
}

Color PhongMaterial::ambient() const
{
    return d->ambient;
}

void PhongMaterial::setAmbient(const Color & c)
{
    d->ambient = c;
}

gl::Texture2D::shared_ptr PhongMaterial::diffuseTexture()
{
    return d->diffuseTex;
}

void PhongMaterial::setDiffuseTexture(gl::Texture2D::shared_ptr & text)
{
    d->diffuseTex = text;
}
