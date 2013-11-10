//
//  PhongMaterial.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

#include <PlastilinaCore/material/PointMaterial.h>

#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/Camera.h>

struct PointMaterial::Impl
{
    VertexShader    vtxShader;
    FragmentShader  fragShader;
    
    CameraNode::shared_ptr              camera;
    std::vector<LightNode::shared_ptr>  lights;
    
    bool            initialized;
    
    Impl() : initialized(false){}
};
PointMaterial::PointMaterial()
: Material(),
d(new Impl)
{
    
}

PointMaterial::~PointMaterial()
{
    
}

void PointMaterial::load()
{
    if (d->initialized) {
        return;
    }
        
    if (!d->vtxShader.compileStatus()) {
        d->vtxShader.loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/plastilinacore/shaders/Point.vs");
        if (!d->vtxShader.compile()){
            std::cerr << "vtxShader: Compilation failed" << std::endl;
            std::cerr << d->vtxShader.infoLog() << std::endl;
        }
    }
    if (!d->fragShader.compileStatus()){
        d->fragShader.loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/plastilinacore/shaders/Point.fs");
        if (!d->fragShader.compile()){
            std::cerr << "fragShader: Compilation failed" << std::endl;
            std::cerr << d->fragShader.infoLog() << std::endl;
        }
    }
    
    shaderProgram()->attachShader(&d->vtxShader);
    shaderProgram()->attachShader(&d->fragShader);
    
    shaderProgram()->bindAttributeLocation(0, "glVertex");
    shaderProgram()->bindAttributeLocation(1, "glColor");
    
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
    
    d->initialized = true;
}

void PointMaterial::unload()
{
    
}


static void
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

static std::vector<LightNode::shared_ptr>
getAllLights(const std::shared_ptr<const SceneNode> & doc)
{
    std::vector<LightNode::shared_ptr> res;
    
    getAllLightsRecursive(doc, res);
    
    return res;
}

static void
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

static CameraNode::shared_ptr
getCamera(const std::shared_ptr<const SceneNode> & doc)
{
    CameraNode::shared_ptr res;
    
    getCameraRecursive(doc, res);
    
    return res;
}

void PointMaterial::setup(const std::shared_ptr<SceneNode> & node)
{
    const std::shared_ptr<const SceneNode> d(node);
	setup(d);
}

void PointMaterial::setup(const std::shared_ptr<const SceneNode> & node)
{
    Eigen::Vector4f camPos(0,0,0,1);
    Eigen::Vector4f lightPos(4,0,0,1);
    
    
    THROW_IF_GLERROR("Failed to set params");
}



