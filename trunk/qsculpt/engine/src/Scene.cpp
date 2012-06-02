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
#include "StdAfx.h"
#include "Scene.h"
#include "SceneNode.h"
#include "geometry/Ray.h"
#include "geometry/Aabb.h"
#include "Octree.h"

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
};

Scene::Scene() : SceneNode(), _d(new Impl())
{
}

Scene::Scene(const std::string& name): SceneNode(name.c_str()), _d(new Impl())
{
}


Scene::~Scene()
{
}

SceneNode::shared_ptr Scene::findByName(const QString& name)
{
    return NULL;
}

SceneNode::shared_ptr Scene::findByIID(uint IID)
{
    size_t index = 0;
    if (itemIndexFromIid(IID, &index)) {
        return item(index).lock();
    }
    return NULL;
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

