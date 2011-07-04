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
#include "ray.h"
#include "Aabb.h"
#include "Octree.h"

class Scene::SceneNodeIterator : public IIterator<SceneNode>
{
    const Scene* _parent;
    mutable int _index;
    
public:
    SceneNodeIterator(const Scene* parent) ;
    
    /**
     * Function to copy iterators of the same type.
     */
    virtual IIterator<SceneNode>* clone() const;
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const;
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    virtual bool hasPrevious() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual SceneNode & next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const SceneNode & next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual SceneNode & previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const SceneNode & previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};

Scene::SceneNodeIterator::SceneNodeIterator(const Scene* parent)
: _parent(parent),_index(0)
{
}

IIterator<SceneNode>* Scene::SceneNodeIterator::clone() const
{
    return new SceneNodeIterator(_parent);
}

bool Scene::SceneNodeIterator::hasNext() const
{
    //return _index >= 0 && _parent->_children.size() > _index;
    return _index >= 0 && _parent->rowCount() > _index;
}

bool Scene::SceneNodeIterator::hasPrevious() const
{
    return false;
}

SceneNode & Scene::SceneNodeIterator::next()
{
    //throw std::runtime_error("Not implemented");
    return *static_cast<SceneNode*>(_parent->item(_index++));
}

const SceneNode & Scene::SceneNodeIterator::next() const
{
    //throw std::runtime_error("Not implemented");
    return *static_cast<SceneNode*>(_parent->item(_index++));
}

SceneNode & Scene::SceneNodeIterator::previous()
{
    throw std::runtime_error("Not implemented");
}

const SceneNode & Scene::SceneNodeIterator::previous() const
{
    throw std::runtime_error("Not implemented");
}

bool Scene::SceneNodeIterator::seek(int pos, IteratorOrigin origin) const
{
    throw std::runtime_error("Not implemented");
}



struct Scene::Impl {
    QScopedPointer<SceneNode> root;
    data::Octree<SceneNode*> octree;
    
    Impl() : root(new SceneNode)
    {
    }
};

Scene::Scene() : _d(new Impl())
{
}

Scene::Scene(const std::string& /*name*/): _d(new Impl())
{
}


Scene::~Scene()
{
}

SceneNode* Scene::findByName(const QString& name)
{
    return NULL;
}

SceneNode* Scene::findByIID(uint IID)
{
    return NULL;
}

bool Scene::intersects(const geometry::Ray &ray, 
                data::ICollection<SceneNode*> *col)
{
    return _d->octree.findIntersect(ray, col);
}

bool Scene::intersects(const geometry::AABB &box, 
                       data::ICollection<SceneNode*> *col)
{
    return _d->octree.findIntersect(box, col);
}

Iterator<SceneNode> Scene::iterator()
{
    return Iterator<SceneNode>(new SceneNodeIterator(this));
}

Iterator<SceneNode> Scene::constIterator() const
{
    return Iterator<SceneNode>(new SceneNodeIterator(this));
}
