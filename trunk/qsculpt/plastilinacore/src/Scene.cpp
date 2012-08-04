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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/Octree.h>

//namespace  {
    struct CenterMassFn {
        Vector3 operator()(SceneNode::weak_ptr) {
            return Vector3();
        }
    };
//};

struct Scene::Impl {
    //data::Octree<SceneNode::weak_ptr,CenterMassFn> octree;
    
    Impl()
    {
    }
    
    SceneNode::shared_ptr findByIidRecursive(SceneNode::shared_ptr root,
                                             uint IID);
};

SceneNode::shared_ptr Scene::Impl::findByIidRecursive(SceneNode::shared_ptr root,
                                                      uint IID)
{
    auto it = root->iterator();
    while (it.hasNext()) {
        auto e = it.next();
        if (e->iid() == IID) {
            return e;
        }
        e = findByIidRecursive(e, IID);
        if (e) {
            return e;
        }
    }
    return NULL;
}

Scene::Scene() : SceneNode(), _d(new Impl())
{
}

Scene::Scene(const std::string& name): SceneNode(name.c_str()), _d(new Impl())
{
}


Scene::~Scene()
{
}

SceneNode::shared_ptr Scene::findByName(const std::string& name)
{
    return NULL;
}

SceneNode::shared_ptr Scene::findByIID(uint IID)
{
    auto thisptr = shared_from_this();
    return _d->findByIidRecursive(thisptr, IID);
}

bool Scene::intersects(const geometry::Ray &ray, 
                       data::ICollection<SceneNode::weak_ptr> *col)
{
    return false;
    //return _d->octree.findIntersect(ray, col);
}

bool Scene::intersects(const geometry::AABB &box, 
                       data::ICollection<SceneNode::weak_ptr> *col)
{
    return false;
    //return _d->octree.findIntersect(box, col);
}

