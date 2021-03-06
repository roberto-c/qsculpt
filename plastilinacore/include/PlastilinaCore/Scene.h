/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
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
#ifndef SCENE_H
#define SCENE_H

#include <PlastilinaCore/ICollection.h>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/SceneNode.h>
#include <string>

class ISurface;
class SceneNode;

namespace geometry
{
class Ray;
class AABB;
};

enum class UpAxis
{
    Z_POS_UP,
    Y_POS_UP,
    X_POS_Up,
    Z_NEG_UP,
    Y_NEG_UP,
    X_NEG_Up
};

/**
 * Contains all the scene data. All 3D objects reference data from the scene.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class DLLEXPORT Scene : public SceneNode
{
  public:
    typedef std::shared_ptr<Scene>       shared_ptr;
    typedef std::shared_ptr<const Scene> const_shared_ptr;
    typedef std::weak_ptr<Scene>         weak_ptr;
    typedef std::unique_ptr<Scene>       ptr;
    typedef std::unique_ptr<const Scene> const_ptr;

    Scene();

    Scene(const std::string& name);

    virtual ~Scene();

    /**
     *
     */
    SceneNode::shared_ptr findByName(const std::string& name) const;

    /**
     * Returns the node with the specified instance ID. NULL if not found.
     */
    SceneNode::shared_ptr findByIID(uint32_t iid) const;

    /**
     *
     */
    CameraNode::shared_ptr createCamera();

    /**
     * Get first camera node.
     */
    CameraNode::shared_ptr getCamera() const;

    /**
     * Returns a list of nodes that intersects a given ray.
     */
    bool intersects(const geometry::Ray&                    ray,
                    data::ICollection<SceneNode::weak_ptr>* col);

    /**
     * Returns a list of nodes that are intersected or contained by an Axis
     * Aligned Bounding Box (AABB)
     */
    bool intersects(const geometry::AABB&                   box,
                    data::ICollection<SceneNode::weak_ptr>* col);

    /**
     * Render all scene
     */
    virtual void render() const;

    virtual void render(RenderState& state) const;

    void loadFromFile(const std::string& filename);

    /**
     * Definition of where is "up" is the world.
     *
     * @returns @enum UpAxis
     */
    UpAxis upAxis() const;

    /**
     * Set where is "up" is the world.
     *
     * @param axis axis that points up.
     */
    void setUpAxis(UpAxis axis);

    /**
     * Returns a list of all lights starting at the node specified.
     * If node is null, then look in all the document.
     */
    std::vector<LightNode::shared_ptr>
    getAllLights(const std::shared_ptr<const SceneNode>& doc = nullptr) const;

  private:
    class SceneNodeIterator;
    struct Impl;

    std::unique_ptr<Impl> _d;

    friend class SceneNodeIterator;
};

#endif
