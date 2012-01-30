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
#include "StdAfx.h"
#include "SceneNode.h"

#include <algorithm>
#include <QAtomicInt>

static QAtomicInt NEXTID;

struct SceneNode::Impl {
    uint                    iid;
    bool                    isSelected;
    SceneNode*              parent;
    std::vector<SceneNode*> children;
    Eigen::Affine3f         transform;
    std::string             name;
    
    Impl() : iid(0), isSelected(false), parent(NULL) {}
};

class SceneNode::SceneNodeIterator : public IIterator<SceneNode>
{
    const SceneNode* _parent;
    mutable int _index;
    
public:
    SceneNodeIterator(const SceneNode* parent) ;
    
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

SceneNode::SceneNodeIterator::SceneNodeIterator(const SceneNode* parent)
: _parent(parent),_index(0)
{
}

IIterator<SceneNode>* SceneNode::SceneNodeIterator::clone() const
{
    return new SceneNodeIterator(_parent);
}

bool SceneNode::SceneNodeIterator::hasNext() const
{
    //return _index >= 0 && _parent->_children.size() > _index;
    //return _index >= 0 && _parent->rowCount() > _index;
    return _index >= 0 && _parent->count() > _index;
}

bool SceneNode::SceneNodeIterator::hasPrevious() const
{
    return false;
}

SceneNode & SceneNode::SceneNodeIterator::next()
{
    //throw std::runtime_error("Not implemented");
    return *static_cast<SceneNode*>(_parent->_d->children[_index++]);
}

const SceneNode & SceneNode::SceneNodeIterator::next() const
{
    //throw std::runtime_error("Not implemented");
    return *static_cast<SceneNode*>(_parent->_d->children[_index++]);
}

SceneNode & SceneNode::SceneNodeIterator::previous()
{
    throw std::runtime_error("Not implemented");
}

const SceneNode & SceneNode::SceneNodeIterator::previous() const
{
    throw std::runtime_error("Not implemented");
}

bool SceneNode::SceneNodeIterator::seek(int pos, IteratorOrigin origin) const
{
    throw std::runtime_error("Not implemented");
}    


SceneNode::SceneNode(const QString& name, SceneNode *parent)
: _d(new Impl)
{
    _d->iid = NEXTID.fetchAndAddRelaxed(1);
    
    //this->setText(name);
    if (parent)
    {
        _d->parent = parent;
        parent->add(this);
    }
    _d->transform = Eigen::Affine3f::Identity();
}

SceneNode::~SceneNode()
{
}

uint SceneNode::iid() const { 
    return _d->iid;
}

std::string SceneNode::name() const {
    return _d->name;
}

void SceneNode::setName(const std::string & name) {
    _d->name = name;
}

void SceneNode::setParent(SceneNode* node)
{
    if (parent() == node) {
        return;
    }
    
    if (parent() != NULL) {
        parent()->remove(this);
    }
    _d->parent = node;
    if (node) {
        node->add(this);
    }
}

SceneNode* SceneNode::parent()
{
    return _d->parent;
}

const SceneNode* SceneNode::parent() const
{
    return _d->parent;
}

bool SceneNode::contains(const SceneNode* child) const 
{
    std::vector<SceneNode*>::iterator it = find(_d->children.begin(), _d->children.end(), child); 
    return it != _d->children.end();
}

void SceneNode::add(SceneNode* child)
{
    if (child == NULL)
        return;
    
    // If child node has a parent, remove child from old parent's children list
    child->setParent(this);
    if (!contains(child)) {
        _d->children.push_back(child);
    }
}

void SceneNode::remove(SceneNode* child)
{
    std::vector<SceneNode*>::iterator it = find(_d->children.begin(), _d->children.end(), child); 
    if (it != _d->children.end())
        _d->children.erase(it);
}

size_t SceneNode::count() const
{
    return _d->children.size();
}

SceneNode* SceneNode::item(size_t index) const
{
    return _d->children[index];
}

bool SceneNode::isSelected() const
{
    return _d->isSelected;
}

void SceneNode::setSelected(bool selected)
{
    _d->isSelected = selected;
}

Eigen::Affine3f SceneNode::transform() const
{ 
    return _d->transform; 
}

Eigen::Affine3f& SceneNode::transform() 
{ 
    return _d->transform; 
}

void SceneNode::setTransform(const Eigen::Affine3f& t)
{
    _d->transform = t;
}

Eigen::Affine3f SceneNode::parentTransform() const
{
    Eigen::Affine3f trans = Eigen::Affine3f::Identity();
    
    SceneNode * node = _d->parent;
    while (node) {
        trans = trans * node->transform();
        node = node->_d->parent;
    }
    return trans;
}

Point3 SceneNode::worldToLocal(Point3 p) const
{
    return transform().inverse() * parentTransform().inverse() * p;
}

Point3 SceneNode::localToWorld(Point3 p) const
{
    return parentTransform() * transform() * p;
}

void SceneNode::render()
{
}

Iterator<SceneNode> SceneNode::iterator() 
{
    return Iterator<SceneNode>(new SceneNodeIterator(this));
}

/**
 * Returns an iterator for the children items in this node
 */
Iterator<SceneNode> SceneNode::constIterator() const 
{
    return Iterator<SceneNode>(new SceneNodeIterator(this));
}


SurfaceNode::SurfaceNode(ISurface *surface, SceneNode *parent)
: SceneNode("", parent)
, _surface(surface)
{
}

SurfaceNode::~SurfaceNode()
{
}

ISurface* SurfaceNode::surface() const
{
    return _surface;
}

void SurfaceNode::setSurface(ISurface *surface)
{
    _surface = surface;
}

