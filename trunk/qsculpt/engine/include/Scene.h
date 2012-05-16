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

#include <Qtcore/QObject>
#include <string>
#include "IIterator.h"
#include "ICollection.h"
#include "SceneNode.h"

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
    //typedef QSharedPointer<Scene>   SharedPtr;
    //typedef QWeakPointer<Scene>     WeakPtr;
    typedef std::shared_ptr<Scene>      SharedPtr;
    typedef std::weak_ptr<Scene>        WeakPtr;
    typedef std::unique_ptr<Scene>      Ptr;
    
    Scene();
    
    Scene(const std::string& name);

    virtual ~Scene();
    
    /**
     *
     */
    SceneNode::SharedPtr findByName(const QString& name);

    /**
     * Returns the node with the specified instance ID. NULL if not found.
     */
    SceneNode::SharedPtr findByIID(uint iid);
    
    /**
     * Returns a list of nodes that intersects a given ray.
     */
    bool intersects(const geometry::Ray &ray, 
                    data::ICollection<SceneNode::WeakPtr> *col);
    
    /**
     * Returns a list of nodes that are intersected or contained by an Axis
     * Aligned Bounding Box (AABB)
     */
    bool intersects(const geometry::AABB &box,
                    data::ICollection<SceneNode::WeakPtr> *col);
    
private:
    class SceneNodeIterator;
    class Impl;
    
    QScopedPointer<Impl> _d;
    
    friend class SceneNodeIterator;
};

#endif

