//
//  TestMaterial.h
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 11/16/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#ifndef __TestApp__TestMaterial__
#define __TestApp__TestMaterial__

#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/opengl/Sampler.h>

class TestMaterial : public Material
{
public:
    TestMaterial();
    
    virtual ~TestMaterial();
    
    virtual void load();
    
    virtual void unload();
    
    virtual void setup(const std::shared_ptr<SceneNode> & doc);
	
	virtual void setup(const std::shared_ptr<const SceneNode> & doc);
	
	/**
	 * gets the texture used for diffuse component.
	 */
	gl::Texture2D::shared_ptr diffuseTexture();
	
	void setDiffuseTexture(gl::Texture2D::shared_ptr & text);
	
	
	/**
	 *
	 */
	 
	
    /**
     * Exponent used for phong lighting model
     */
    float exponent() const;
    
    /**
     * Set the exponent for Phong lighting model
     */
    void setExponent(float exp);
    
    /**
     * Returns the color of the specular component used in shading color computation.
     */
    Color specular() const;
    
    /**
     * Sets the color used for specular component used for shading color computation.
     */
    void setSpecular(const Color & c);
    
    /**
     * Returns the diffuse color component used in shading color computation.
     */
    Color diffuse() const;
    
    /**
     * Sets the diffuse color used for shading color computation.
     */
    void setDiffuse(const Color & c);
    
    /**
     * Returns the diffuse color component used in shading color computation.
     */
    Color ambient() const;
    
    /**
     * Sets the diffuse color used for shading color computation.
     */
    void setAmbient(const Color & c);
    
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif /* defined(__TestApp__TestMaterial__) */
