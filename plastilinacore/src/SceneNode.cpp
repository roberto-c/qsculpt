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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/SceneNode.h>

#include <algorithm>
#include <atomic>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Camera.h>

static std::atomic_int NEXTID;

struct SceneNode::Impl {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    uint32_t                            iid;
    NodeType							nodeType;
    bool                                isSelected;
    SceneNode::weak_ptr                 parent;
    std::vector<SceneNode::shared_ptr>  children;
    Eigen::Affine3f                     transform;
    std::string                         name;
    Eigen::IOFormat 					octaveFmt;
    geometry::AABB                      bbox;
    
    Impl(const std::string & name, NodeType nodeType = NT_Normal) :
     iid(0), nodeType(nodeType), isSelected(false),name(name),
        octaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]")
    {
        TRACEFUNCTION("");
    }
    
    ~Impl() {
        TRACEFUNCTION("");
    }
    
    void dump(int indent = 0) const {
        std::string p, d;
        for (int i = 0; i < indent; ++i) {
            p += "    ";
        }
        std::cout << p << "+--- Name:" << name << " (" << iid << ")" << std::endl;
        std::cout << p << "   | NodeType:" << nodeType << std::endl;
        std::cout << p << "   | T: \n" << transform.matrix().format(octaveFmt) << std::endl;
        std::cout << p << "   | Children (" << children.size() << "): " << std::endl;
        for(auto e : children) {
            e->_d->dump(indent+1);
        }
    }
};

class SceneNode::SceneNodeIterator : public IIterator<SceneNode>
{
public:
    typedef SceneNode::shared_ptr    shared_ptr;
    typedef SceneNode::weak_ptr      weak_ptr;
    typedef SceneNode::ptr          ptr;
    
    SceneNode::const_shared_ptr     _parent;
    mutable unsigned int _index;
    
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
    virtual shared_ptr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const_shared_ptr next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const_shared_ptr previous() const;
    
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

SceneNode::SceneNodeIterator::shared_ptr SceneNode::SceneNodeIterator::next()
{
    //throw std::runtime_error("Not implemented");
    SceneNode::shared_ptr ptr = _parent->_d->children[_index++];
    return  ptr;
}

SceneNode::SceneNodeIterator::const_shared_ptr SceneNode::SceneNodeIterator::next() const
{
    //throw std::runtime_error("Not implemented");
    SceneNode::shared_ptr ptr = _parent->_d->children[_index++];
    return  ptr;
}

SceneNode::SceneNodeIterator::shared_ptr SceneNode::SceneNodeIterator::previous()
{
    throw std::runtime_error("Not implemented");
}

SceneNode::SceneNodeIterator::const_shared_ptr SceneNode::SceneNodeIterator::previous() const
{
    throw std::runtime_error("Not implemented");
}

bool SceneNode::SceneNodeIterator::seek(int pos, IteratorOrigin origin) const
{
    throw std::runtime_error("Not implemented");
}    

class SceneNode::SceneNodeTreeIterator : public IIterator<SceneNode>
{
public:
    typedef SceneNode::shared_ptr    shared_ptr;
    typedef SceneNode::weak_ptr      weak_ptr;
    typedef SceneNode::ptr           ptr;
    
    mutable SceneNode::const_shared_ptr     _parent;
    mutable std::vector<uint32_t>		_levelStack;
    mutable unsigned int _nextIndex;
    
public:
    SceneNodeTreeIterator(const SceneNode::const_weak_ptr & parent) ;
    
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
    virtual shared_ptr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const_shared_ptr next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const_shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};

SceneNode::SceneNodeTreeIterator::SceneNodeTreeIterator(const SceneNode::const_weak_ptr & parent)
: _parent(parent),_nextIndex(0)
{
}

IIterator<SceneNode>* SceneNode::SceneNodeTreeIterator::clone() const
{
    SceneNodeTreeIterator * it = new SceneNodeTreeIterator(_parent);
    if (it) {
        it->_levelStack = _levelStack;
        it->_parent = _parent;
    }
    return it;
}

bool SceneNode::SceneNodeTreeIterator::hasNext() const
{
    if (!_parent) {
        throw std::runtime_error("Parent is null");
    }
//    bool res = _nextIndex >= 0 && ((_parent && _parent->count() > _nextIndex) || _levelStack.size() > 0);
    bool res = _nextIndex >= 0 && ((_parent && _parent->count() > _nextIndex));
    std::cerr << "NextIndex: " << _nextIndex << " ParentCount: " << ((_parent) ? _parent->count() : -1 ) << "\n";
    return res;
}

bool SceneNode::SceneNodeTreeIterator::hasPrevious() const
{
    return false;
}

SceneNode::SceneNodeTreeIterator::const_shared_ptr SceneNode::SceneNodeTreeIterator::next() const
{
    //throw std::runtime_error("Not implemented");
    auto ret = _parent->item(_nextIndex).lock();
    _nextIndex++;
    if (ret->count() > 0) {
        _levelStack.push_back(_nextIndex);
        _nextIndex = 0;
        _parent = ret;
    }
    
    while(_nextIndex >= _parent->count()) {
        auto p = _parent->parent().lock();
        if (p) {
            _nextIndex = _levelStack.back();
            _levelStack.pop_back();
            _parent = _parent->parent().lock();
        } else {
            break;
        }
    }
    std::cerr << "RetType:" << ret->nodeType() << " NextIndex: " << _nextIndex << " ParentCount: " << ((_parent) ? _parent->count() : -1 ) << "\n";
    return  ret;
}

SceneNode::SceneNodeTreeIterator::shared_ptr SceneNode::SceneNodeTreeIterator::next()
{
    const SceneNode::SceneNodeTreeIterator& p = static_cast<const SceneNode::SceneNodeTreeIterator&>(*this);
    return  std::const_pointer_cast<SceneNode>(p.next());
}

SceneNode::SceneNodeTreeIterator::shared_ptr SceneNode::SceneNodeTreeIterator::previous()
{
    throw std::runtime_error("Not implemented");
}

SceneNode::SceneNodeTreeIterator::const_shared_ptr SceneNode::SceneNodeTreeIterator::previous() const
{
    throw std::runtime_error("Not implemented");
}

bool SceneNode::SceneNodeTreeIterator::seek(int pos, IteratorOrigin origin) const
{
    throw std::runtime_error("Not implemented");
}


SceneNode::SceneNode(const std::string& name)
: enable_shared_from_this(),
  _d(new Impl(name))
{
    TRACEFUNCTION(("Name : " + name));
    _d->iid = NEXTID.fetch_add(1);
    
    _d->transform = Eigen::Affine3f::Identity();
}

SceneNode::SceneNode(const std::string & name, NodeType nodeType)
: enable_shared_from_this(),
  _d(new Impl(name, nodeType))
{
    TRACEFUNCTION(("Name : " + name));
    _d->iid = NEXTID.fetch_add(1);
    
    _d->transform = Eigen::Affine3f::Identity();
}

void SceneNode::updateParentBBox()
{
    auto parentnode = this->parent().lock();
    if (parentnode) {
        parentnode->boundingBox().extend(this->boundingBox());
    }
}

SceneNode::~SceneNode()
{
    TRACEFUNCTION(("Name : " + name()));
}

uint32_t SceneNode::iid() const { 
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

void SceneNode::setParent(SceneNode::weak_ptr node)
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

SceneNode::weak_ptr SceneNode::parent()
{
    assert(_d);
    return _d->parent;
}

const SceneNode::weak_ptr SceneNode::parent() const
{
    assert(_d);
    return _d->parent;
}

bool SceneNode::contains(const SceneNode::weak_ptr child) const 
{
    return itemIndex(child, NULL);
}

bool SceneNode::itemIndex(const SceneNode::weak_ptr child, size_t * index) const
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

bool SceneNode::itemIndexFromIid(uint32_t childIID, size_t * index) const
{
    assert(_d);
    
    size_t count = _d->children.size();
    for (size_t i = 0; i < count; ++i) {
        auto myc = _d->children[i]->iid();
        if (myc == childIID) {
            if (index) *index = i;
            return true;
        }
    }
    return false;
}

void SceneNode::add(SceneNode::weak_ptr child)
{
    assert(_d);
    
    auto ptr = child.lock();
    if (!ptr) {
        return;
    }
    
    // Only add the node if it doe not exist in the children list
    if (!contains(child)) {
        _d->children.push_back(ptr);
    } 
    
    auto thisptr= shared_from_this();
    ptr->setParent(thisptr);
}

void SceneNode::remove(SceneNode::weak_ptr child)
{
    assert(_d);
    
    auto ptr = child.lock();
    if (ptr) {
        uint32_t iid = ptr->iid();
        remove(iid);
    }
}

void SceneNode::remove(uint32_t iid)
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
            ptr->setParent(SceneNode::weak_ptr());
        }
        _d->children.erase(it);
    }
}

size_t SceneNode::count() const
{
    assert(_d);
    return _d->children.size();
}

SceneNode::weak_ptr SceneNode::item(size_t index) const
{
    assert(_d);
//    assert(index < _d->children.size());
    if (index >= _d->children.size())
        return SceneNode::weak_ptr();
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
    
    SceneNode::shared_ptr node = _d->parent.lock();
    while (node) {
        trans = node->transform() * trans;
        node = node->_d->parent.lock();
    }
    return trans;
}

geometry::AABB SceneNode::boundingBox() const
{
    return _d->bbox;
}

geometry::AABB & SceneNode::boundingBox()
{
    return _d->bbox;
}

Point3 SceneNode::worldToLocal(Point3 p) const
{
//    return transform().inverse() * parentTransform().inverse() * p;
    return (transform() * parentTransform()).inverse() * p;
}

Point3 SceneNode::localToWorld(Point3 p) const
{
    return parentTransform() * transform() * p;
}

void SceneNode::render(const RenderState * state) const
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

Iterator<SceneNode> SceneNode::treeIterator() const
{
    auto ptr = shared_from_this();
    Iterator<SceneNode> it(new SceneNodeTreeIterator(ptr));
    return it;
}

Iterator<SceneNode> SceneNode::treeIterator()
{
    auto ptr = shared_from_this();
    Iterator<SceneNode> it(new SceneNodeTreeIterator(ptr));
    return it;
}


void SceneNode::dump()
{
    _d->dump();
}

SurfaceNode::SurfaceNode(ISurface *surface,
    const std::string & name)
: SceneNode(name, NT_Surface),
  surface_(surface)
{
    if (surface)
    {
        boundingBox().extend(surface->boundingBox());
        updateParentBBox();
    }
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
    boundingBox().extend(surface->boundingBox());
    updateParentBBox();
}

std::shared_ptr<Material> SurfaceNode::material() const
{
    return material_;
}

/**
 * Set the material used for this surface.
 */
void SurfaceNode::setMaterial(const std::shared_ptr<Material> & mat)
{
    material_ = mat;
}

void SurfaceNode::render(RenderState & state) const
{
    this->surface_->renderable()->render(state);
}

LightNode::LightNode(const std::string & name)
: SceneNode(name, NT_Light)
{
    
}

LightNode::~LightNode()
{
    
}

void LightNode::setColor(const Eigen::Vector4f & color)
{
    
}

Eigen::Vector4f LightNode::color() const
{
    return Eigen::Vector4f(1, 1, 1, 1);
}


struct CameraNode::Impl
{
    std::shared_ptr<Camera> camera;
};

CameraNode::CameraNode(const std::shared_ptr<Camera> & cam,
                       const std::string & name)
: SceneNode(name, NT_Camera),
    d(new Impl)
{
    if (d) {
        setCamera(cam);
    }
}

CameraNode::~CameraNode()
{
    
}

void CameraNode::setCamera(const std::shared_ptr<Camera> & cam)
{
    d->camera = cam;
    add(cam);
}

std::shared_ptr<Camera> CameraNode::camera() const
{
    return d->camera;
}

EffectNode::EffectNode(const std::shared_ptr<Material>& material, const std::string & name)
{
}

EffectNode::~EffectNode()
{
}

std::shared_ptr<Material> EffectNode::material() const
{
    return std::shared_ptr<Material>(nullptr);
}

void EffectNode::setMaterial(const std::shared_ptr<Material>& material)
{
}
