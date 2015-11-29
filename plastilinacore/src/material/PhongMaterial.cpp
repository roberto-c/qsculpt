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
    Color           specular;
    Color           diffuse;
    Color           ambient;
    float           exponent;
    
    VertexShader    vtxShader;
    FragmentShader  fragShader;
    
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
            std::cerr << "vtxShader: Compilation failed" << std::endl;
            std::cerr << d->vtxShader.infoLog() << std::endl;
            throw core::GlException("Failed to compile vertex shader", glGetError());
        }
    }
    if (!d->fragShader.compileStatus()){
		std::string path = mgr.findResourcePath("Phong", "fs", "shaders");
        d->fragShader.loadFromFile(path);
        if (!d->fragShader.compile()){
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
            << " ("<< std::hex << type << std::dec << "[" << size <<"])\n";
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


void
getAllLightsRecursive(const std::shared_ptr<const SceneNode> & scene,
                      std::vector<LightNode::shared_ptr> & container)
{
    auto it = scene->constIterator();
    while(it.hasNext()) {
        SceneNode::shared_ptr child = it.next();
        LightNode::shared_ptr light = std::dynamic_pointer_cast<LightNode>(child);
        if (light) {
            container.push_back(light);
        }
        getAllLightsRecursive(child, container);
    }
}

std::vector<LightNode::shared_ptr>
getAllLights(const std::shared_ptr<const SceneNode> & doc)
{
    std::vector<LightNode::shared_ptr> res;
    
    getAllLightsRecursive(doc, res);
    
    return res;
}

void
getCameraRecursive(const std::shared_ptr<const SceneNode> & scene,
                   CameraNode::shared_ptr & container)
{
    if (container) return;
    
    auto it = scene->constIterator();
    while(it.hasNext()) {
        SceneNode::shared_ptr child = it.next();
        CameraNode::shared_ptr cam = std::dynamic_pointer_cast<CameraNode>(child);
        if (cam) {
            container = cam;
        } else {
            getCameraRecursive(child, container);
        }
    }
}

CameraNode::shared_ptr
getCamera(const std::shared_ptr<const SceneNode> & doc)
{
    CameraNode::shared_ptr res;
    
    getCameraRecursive(doc, res);
    
    return res;
}

void PhongMaterial::setup(const std::shared_ptr<SceneNode> & doc)
{
	const std::shared_ptr<const SceneNode> d(doc);
    setup(d);
}

void PhongMaterial::setup(const std::shared_ptr<const SceneNode> & doc)
{
    Eigen::Vector4f camPos(0,0,0,1);
    Eigen::Vector4f lightPos(4,0,0,1);
    
    if (doc) {
        d->lights = getAllLights(doc);
        if (d->lights.size() > 0) {
            
        }
    }
    
    GLint diff = shaderProgram()->uniformLocation("diffuseColor");
    GLint spec = shaderProgram()->uniformLocation("specularColor");
    GLint amb = shaderProgram()->uniformLocation("ambientColor");
    GLint exp = shaderProgram()->uniformLocation("exponent");
    GLint lightPosId = shaderProgram()->uniformLocation("lightPosition");
    
    if (diff != -1) shaderProgram()->setUniform(diff, d->diffuse.data());
    if (spec != -1) shaderProgram()->setUniform(spec, d->specular.data());
    if (amb != -1) shaderProgram()->setUniform(amb, d->ambient.data());
    if (exp != -1) shaderProgram()->setUniform(exp, d->exponent);
    if (lightPosId != -1) shaderProgram()->setUniform(lightPosId, lightPos);
    
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

