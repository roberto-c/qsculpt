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


#include <string>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/ICollection.h>
#include <PlastilinaCore/SceneNode.h>

class ISurface;
class SceneNode;

namespace geometry {
    class Ray;
    class AABB;
};

/**
 * Contains all the scene data. All 3D objects reference data from the scene.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class Scene : public SceneNode {
public:
    //typedef QSharedPointer<Scene>   shared_ptr;
    //typedef QWeakPointer<Scene>     weak_ptr;
    typedef std::shared_ptr<Scene>      shared_ptr;
    typedef std::weak_ptr<Scene>        weak_ptr;
    typedef std::unique_ptr<Scene>      Ptr;
    
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
    SceneNode::shared_ptr findByIID(uint iid) const;
    
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
    bool intersects(const geometry::Ray &ray, 
                    data::ICollection<SceneNode::weak_ptr> *col);
    
    /**
     * Returns a list of nodes that are intersected or contained by an Axis
     * Aligned Bounding Box (AABB)
     */
    bool intersects(const geometry::AABB &box,
                    data::ICollection<SceneNode::weak_ptr> *col);
	
	/**
	 * Render all scene
	 */
	void render() const;
    
private:
    class SceneNodeIterator;
    class Impl;
    
    std::unique_ptr<Impl> _d;
    
    friend class SceneNodeIterator;
};

#endif

