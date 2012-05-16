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
#include <QtCore/QAtomicInt>

static QAtomicInt NEXTID;

struct SceneNode::Impl {
    uint                                iid;
    bool                                isSelected;
    SceneNode::WeakPtr                  parent;
    std::vector<SceneNode::SharedPtr>   children;
    Eigen::Affine3f                     transform;
    std::string                         name;
    
    Impl() : iid(0), isSelected(false) {
        qDebug() << __PRETTY_FUNCTION__;
    }
    
    ~Impl() {
        qDebug() << __PRETTY_FUNCTION__;
    }
};

class SceneNode::SceneNodeIterator : public IIterator<SceneNode>
{
public:
    typedef SceneNode::SharedPtr    SharedPtr;
    typedef SceneNode::WeakPtr      WeakPtr;
    typedef SceneNode::Ptr          Ptr;
    
    SceneNode::const_shared_ptr     _parent;
    mutable int _index;
    
public:
    SceneNodeIterator(const SceneNode::const_weak_ptr & parent) ;
    
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
    virtual SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const SharedPtr next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual SharedPtr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const SharedPtr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};

SceneNode::SceneNodeIterator::SceneNodeIterator(const SceneNode::const_weak_ptr & parent)
: _parent(parent),_index(0)
{
}

IIterator<SceneNode>* SceneNode::SceneNodeIterator::clone() const
{
    return new SceneNodeIterator(_parent);
}

bool SceneNode::SceneNodeIterator::hasNext() const
{
    if (!_parent) {
        throw std::runtime_error("Parent is null");
    }
    return _index >= 0 && _parent->count() > _index;
}

bool SceneNode::SceneNodeIterator::hasPrevious() const
{
    return false;
}

SceneNode::SceneNodeIterator::SharedPtr SceneNode::SceneNodeIterator::next()
{
    //throw std::runtime_error("Not implemented");
    SceneNode::SharedPtr ptr = _parent->_d->children[_index++];
    return  ptr;
}

const SceneNode::SceneNodeIterator::SharedPtr SceneNode::SceneNodeIterator::next() const
{
    //throw std::runtime_error("Not implemented");
    SceneNode::SharedPtr ptr = _parent->_d->children[_index++];
    return  ptr;
}

SceneNode::SceneNodeIterator::SharedPtr SceneNode::SceneNodeIterator::previous()
{
    throw std::runtime_error("Not implemented");
}

const SceneNode::SceneNodeIterator::SharedPtr SceneNode::SceneNodeIterator::previous() const
{
    throw std::runtime_error("Not implemented");
}

bool SceneNode::SceneNodeIterator::seek(int pos, IteratorOrigin origin) const
{
    throw std::runtime_error("Not implemented");
}    


SceneNode::SceneNode(const QString& name)
: _d(new Impl)
{
    _d->iid = NEXTID.fetchAndAddRelaxed(1);
    
    //this->setText(name);
    _d->transform = Eigen::Affine3f::Identity();
}

SceneNode::~SceneNode()
{
    qDebug() << __PRETTY_FUNCTION__ << ": " << name().c_str();
}

uint SceneNode::iid() const { 
    assert(_d);
    return _d->iid;
}

std::string SceneNode::name() const {
    assert(_d);
    return _d->name;
}

void SceneNode::setName(const std::string & name) {
    assert(_d);
    _d->name = name;
}

void SceneNode::setParent(SceneNode::WeakPtr node)
{
    assert(_d);
    auto p = parent().lock();
    auto n = node.lock();
    
    if (p == n) {
        return;
    }
    
    if (p) {
        p->remove(iid());
    }
    _d->parent = node;
}

SceneNode::WeakPtr SceneNode::parent()
{
    assert(_d);
    return _d->parent;
}

const SceneNode::WeakPtr SceneNode::parent() const
{
    assert(_d);
    return _d->parent;
}

bool SceneNode::contains(const SceneNode::WeakPtr child) const 
{
    return itemIndex(child, NULL);
}

bool SceneNode::itemIndex(const SceneNode::WeakPtr child, size_t * index) const
{
    assert(_d);
    auto c = child.lock();
    size_t count = _d->children.size();
    for (size_t i = 0; i < count; ++i) {
        auto myc = _d->children[i];
        if (myc == c) {
            if (index) *index = i;
            return true;
        }
    }
    return false;
}

void SceneNode::add(SceneNode::WeakPtr child)
{
    assert(_d);
    
    auto ptr = child.lock();
    if (!ptr) {
        return;
    }
    
    // If child node has a parent, remove child from old parent's children list
    if (!contains(child)) {
        _d->children.push_back(ptr);
    } 
    
    auto thisptr= shared_from_this();
    ptr->setParent(thisptr);
}

void SceneNode::remove(SceneNode::WeakPtr child)
{
    assert(_d);
    
    auto ptr = child.lock();
    if (ptr) {
        uint iid = ptr->iid();
        remove(iid);
    }
}

void SceneNode::remove(uint iid)
{
    assert(_d);
    auto it = _d->children.begin();
    for (;it != _d->children.end(); ++it) {
        auto ptr = (*it);
        if (ptr && ptr->iid() == iid) {
            break;
        }
    }
    if (it != _d->children.end()) {
        auto ptr = (*it);
        if (ptr) {
            ptr->setParent(SceneNode::WeakPtr());
        }
        _d->children.erase(it);
    }
}

size_t SceneNode::count() const
{
    assert(_d);
    return _d->children.size();
}

SceneNode::WeakPtr SceneNode::item(size_t index) const
{
    assert(_d);
    return _d->children[index];
}

bool SceneNode::isSelected() const
{
    assert(_d);
    return _d->isSelected;
}

void SceneNode::setSelected(bool selected)
{
    assert(_d);
    _d->isSelected = selected;
}

Eigen::Affine3f SceneNode::transform() const
{ 
    assert(_d);
    return _d->transform; 
}

Eigen::Affine3f& SceneNode::transform() 
{ 
    assert(_d);
    return _d->transform; 
}

void SceneNode::setTransform(const Eigen::Affine3f& t)
{
    assert(_d);
    _d->transform = t;
}

Eigen::Affine3f SceneNode::parentTransform() const
{
    assert(_d);
    Eigen::Affine3f trans = Eigen::Affine3f::Identity();
    
    SceneNode::SharedPtr node = _d->parent.lock();
    while (node) {
        trans = trans * node->transform();
        node = node->_d->parent.lock();
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
    auto ptr = shared_from_this();
    Iterator<SceneNode> it(new SceneNodeIterator(ptr));
    return it;
}

/**
 * Returns an iterator for the children items in this node
 */
Iterator<SceneNode> SceneNode::constIterator() const 
{
    auto ptr = shared_from_this();
    Iterator<SceneNode> it(new SceneNodeIterator(ptr));
    return it;
}


SurfaceNode::SurfaceNode(ISurface *surface)
: SceneNode(""),
 surface_(surface)
{
}

SurfaceNode::~SurfaceNode()
{
}

ISurface* SurfaceNode::surface() const
{
    return surface_;
}

ISurface* SurfaceNode::surface()
{
    return surface_;
}

void SurfaceNode::setSurface(ISurface *surface)
{
    surface_ = surface;
}

