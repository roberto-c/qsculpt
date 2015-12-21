//
//  TestMaterial.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 11/16/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//
#include "stable.h"
#include "TestMaterial.h"
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/ResourcesManager.h>


struct TestMaterial::Impl
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

TestMaterial::TestMaterial()
: Material(),
d(new Impl)
{
    
}

TestMaterial::~TestMaterial()
{
    
}

void TestMaterial::load()
{
    if (d->initialized) {
        return;
    }
	
	ResourcesManager mgr;
    if (!d->vtxShader.compileStatus()) {
		std::string path = mgr.findResourcePath("TestMaterial", "vs");
        d->vtxShader.loadFromFile(path);
        if (!d->vtxShader.compile()){
            std::cerr << "vtxShader: Compilation failed" << std::endl;
            std::cerr << d->vtxShader.infoLog() << std::endl;
        }
    }
    if (!d->fragShader.compileStatus()){
		std::string path = mgr.findResourcePath("TestMaterial", "fs");
        d->fragShader.loadFromFile(path);
        if (!d->fragShader.compile()){
            std::cerr << "fragShader: Compilation failed" << std::endl;
            std::cerr << d->fragShader.infoLog() << std::endl;
        }
    }
    
    shaderProgram()->attachShader(&d->vtxShader);
    shaderProgram()->attachShader(&d->fragShader);
    
    shaderProgram()->bindAttributeLocation(0, "glVertex");
    shaderProgram()->bindAttributeLocation(1, "glNormal");
    shaderProgram()->bindAttributeLocation(2, "glColor");
	shaderProgram()->bindAttributeLocation(3, "glTexCoord");
    
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

void TestMaterial::unload()
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

void TestMaterial::setup(const std::shared_ptr<SceneNode> & doc)
{
	const std::shared_ptr<const SceneNode> d(doc);
    setup(d);
}

void TestMaterial::setup(const std::shared_ptr<const SceneNode> & doc)
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


gl::Texture2D::shared_ptr TestMaterial::diffuseTexture()
{
	return d->diffuseTex;
}

void TestMaterial::setDiffuseTexture(gl::Texture2D::shared_ptr & text)
{
	d->diffuseTex = text;
}


float TestMaterial::exponent() const
{
    return d->exponent;
}

void TestMaterial::setExponent(float exp)
{
    d->exponent = exp;
}

Color TestMaterial::specular() const
{
    return d->specular;
}

void TestMaterial::setSpecular(const Color & c)
{
    d->specular = c;
}

Color TestMaterial::diffuse() const
{
    return d->diffuse;
}

void TestMaterial::setDiffuse(const Color & c)
{
    d->diffuse = c;
}

Color TestMaterial::ambient() const
{
    return d->ambient;
}

void TestMaterial::setAmbient(const Color & c)
{
    d->ambient = c;
}

