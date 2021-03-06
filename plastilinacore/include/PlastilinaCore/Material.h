/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores                      *
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

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <Eigen/Geometry>
#include <memory>

class GlslProgram;
class LightNode;
class SceneNode;
class IDocument;

/**
 * Defines the appeareance of the surface or a renderable object.
 *
 * Each material is composed by a shader objects for real time rendring using
 * OpenGL.
 */
class DLLEXPORT Material
{
  public:
    Material();

    virtual ~Material();

    /**
     * Returns the instance ID of the material object.
     */
    int iid() const;

    /**
     * Setup all data to render an object using this material.
     *
     * This function can be used to load static and external resources used
     * by the material, like shader source code and additional resources.
     *
     * This function should be called just once, at the initialization of the
     * material.
     */
    virtual void load() = 0;

    /**
     * Releases all data and resources used by this material.
     */
    virtual void unload() = 0;

    /**
     * Sets up material with variables and properties needed
     *
     * This function must be implemented by subclasses to bind all necesary
     * resources, like shaders, textures, lighting parameters, etc.
     */
    virtual void setup(const std::shared_ptr<SceneNode>& doc) = 0;

    virtual void setup(const std::shared_ptr<const SceneNode>& doc) = 0;

    /**
     * Gets the shader program used for implementing the material.
     */
    GlslProgram* shaderProgram() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> d_;
};

enum MaterialPropertyType
{
    Void = 0,
    Int  = 1,
    Int16,
    Int32,
    Int64,
    Float,
    Double,
    Vec2,
    Vec3,
    Vec4,
    Mat2x2,
    Mat2x3,
    Mat2x4,
    Mat3x2,
    Mat3x3,
    Mat3x4,
    Mat4x2,
    Mat4x3,
    Mat4x4,
    Texture1D,
    Texture2D,
    Texture3D
};

struct DLLEXPORT MaterialProperty
{
    MaterialPropertyType type;
    bool                 pointer;
    bool                 input;
    bool                 ouput;
    bool                 uniform;
    bool                 varying;
};

class DLLEXPORT MaterialNode
{
  public:
    /**
     *
     */
    MaterialNode();

    virtual ~MaterialNode();

    bool connect(const std::string& name, MaterialNode* node2,
                 const std::string& inputName);

  protected:
    bool registerProperty(const std::string& name, MaterialProperty type);
};

class DLLEXPORT CookTorrance : public Material
{
    struct Impl;
    std::unique_ptr<Impl> d;

  public:
    CookTorrance();
    ~CookTorrance();

    virtual void load();
    virtual void unload();
    virtual void setup(const std::shared_ptr<SceneNode>& doc);
    virtual void setup(const std::shared_ptr<const SceneNode>& doc);

    void setColor(const Eigen::Vector4f& c);
    Eigen::Vector4f color();

    void setLight(std::shared_ptr<LightNode>& p);
    std::shared_ptr<LightNode> light();
};

#endif
