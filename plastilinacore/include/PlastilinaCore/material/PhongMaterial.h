//
//  PhongMaterial.h
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

#ifndef __TestApp__PhongMaterial__
#define __TestApp__PhongMaterial__

#include <iostream>

#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/opengl/Sampler.h>
#include <PlastilinaCore/opengl/Texture.h>

class DLLEXPORT PhongMaterial : public Material
{
  public:
    PhongMaterial();

    virtual ~PhongMaterial();

    virtual void load();

    virtual void unload();

    virtual void setup(const std::shared_ptr<SceneNode>& doc);

    virtual void setup(const std::shared_ptr<const SceneNode>& doc);

    /**
     * Exponent used for phong lighting model
     */
    float exponent() const;

    /**
     * Set the exponent for Phong lighting model
     */
    void setExponent(float exp);

    /**
     * Returns the color of the specular component used in shading color
     * computation.
     */
    Color specular() const;

    /**
     * Sets the color used for specular component used for shading color
     * computation.
     */
    void setSpecular(const Color& c);

    /**
     * Returns the diffuse color component used in shading color computation.
     */
    Color diffuse() const;

    /**
     * Sets the diffuse color used for shading color computation.
     */
    void setDiffuse(const Color& c);

    /**
     * Returns the diffuse color component used in shading color computation.
     */
    Color ambient() const;

    /**
     * Sets the diffuse color used for shading color computation.
     */
    void setAmbient(const Color& c);

    /**
     * Returns the texture used for diffuse color.
     */
    gl::Texture2D::shared_ptr diffuseTexture();

    /**
     * Sets the texture to use for diffuse color.
     * if set to null, the material will use the regular diffuse color.
     */
    void setDiffuseTexture(gl::Texture2D::shared_ptr& text);

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif /* defined(__TestApp__PhongMaterial__) */
