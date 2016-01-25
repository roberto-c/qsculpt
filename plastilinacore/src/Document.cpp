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
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Document.h>

#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/subdivision/Sphere.h>
#include <PlastilinaCore/Mesh.h>
#include <PlastilinaCore/Quad.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/Scene.h>

#define PLASTILINA_TRACE_DISABLE = 1
#include <PlastilinaCore/Logging.h>

static std::atomic_int NEXT_ID(0);

class SurfaceIterator : public IIterator<ISurface>
{
public:
    typedef std::shared_ptr<ISurface>       shared_ptr;
    typedef std::shared_ptr<const ISurface> const_shared_ptr;
    typedef std::weak_ptr<ISurface>         weak_ptr;
    typedef std::unique_ptr<ISurface>       ptr;
    typedef std::unique_ptr<const ISurface> const_ptr;
    
private:
    Document::shared_ptr _doc;

public:
    SurfaceIterator(Document::weak_ptr surface) ;
    
    ~SurfaceIterator() ;

    /**
     * Function to copy iterators of the same type.
     */
    virtual IIterator<ISurface>* clone() const;

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

class SceneIterator : public IIterator<SceneNode>
{
public:
    typedef std::shared_ptr<SceneNode>          shared_ptr;
    typedef std::shared_ptr<const SceneNode>    const_shared_ptr;
    typedef std::weak_ptr<SceneNode>            weak_ptr;
    typedef std::weak_ptr<const SceneNode>      const_weak_ptr;
    typedef std::unique_ptr<SceneNode>          ptr;
    typedef std::unique_ptr<const SceneNode>    const_ptr;
    
private:
    Document::const_shared_ptr  _doc;
    Scene::shared_ptr            _scene;
    mutable size_t               _current;
    
public:
    SceneIterator(const Document::const_weak_ptr & doc) ;
    
    ~SceneIterator() ;
    
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


struct Document::Impl {
    Scene::shared_ptr       scene;
    
    Impl() :scene(std::make_shared<Scene>()) 
    {
    }
    
    void getSelectedNodesRecursive(const SceneNode::shared_ptr & node,
                                   std::vector<SceneNode::weak_ptr> & list) 
    {
        Iterator<SceneNode> it = node->constIterator();
        while (it.hasNext()) {
            auto n = it.next();
            if (n->isSelected()) {
                list.push_back(n);
            }
            getSelectedNodesRecursive(n, list);
        }
    }
};

Document::Document() : _d(new Impl)
{
}


Document::~Document() 
{    
}

void Document::loadFile(const std::string& fileName)
{
    _d->scene->loadFromFile(fileName);
}

void Document::saveFile(const std::string& fileName)
{
    NOT_IMPLEMENTED;
}



void Document::selectObject(int iid)
{
    auto node = findItem(iid);
    if (node) {
        node->setSelected(true);
    }
}

std::vector<SceneNode::weak_ptr> Document::getSelectedObjects() const
{
    std::vector<SceneNode::weak_ptr> selectedObjectList;
    selectedObjectList.clear();

    _d->getSelectedNodesRecursive(_d->scene, selectedObjectList);
    return selectedObjectList;
}

Scene::weak_ptr Document::scene()
{
    return _d->scene;
}

Scene::weak_ptr Document::scene() const
{
    return _d->scene;
}


SceneNode::shared_ptr Document::index (int row, 
                               const SceneNode::shared_ptr & parent ) const
{
    TRACEFUNCTION("");
//    std::cerr << "Arg1: " << row << " Arg2:" << column << " Arg3:" << parent;
    assert(_d && _d->scene);
    
    SceneNode::shared_ptr p = parent;
    if (p) {
        p = p->item(row).lock();
    } else {
        p = _d->scene->item(row).lock();
    }
    return p;
}

SceneNode::shared_ptr Document::parent ( const SceneNode::shared_ptr & index ) const
{
    TRACEFUNCTION("");
//    std::cerr << "Arg1: " << index;
    assert(_d && _d->scene);
    
    SceneNode::shared_ptr ret;
    if (!index) {
//        std::cerr << ret;
        return ret;
    }
    
    SceneNode::shared_ptr ptr = index;
    SceneNode::shared_ptr parent, grandparent;
    size_t row = -1;
    
    // we need to obtaing the position of parent below its parent (i.e. the
    // grand parent of index)
    if (ptr) {
        parent = ptr->parent().lock();
        if (parent) {
            grandparent = parent->parent().lock();
            if (grandparent) {
                size_t n = 0;
                if (grandparent->itemIndex(parent, &n)) {
                    row = n;
                }
            }
        }
    } 
    if (parent && row > -1) {
        ret = parent;
    }
//    std::cerr << ret;
    return ret;
}

size_t Document::childrenCount ( const SceneNode::shared_ptr & parent ) const
{
    TRACEFUNCTION("");
//    std::cerr << "Arg1: " << parent;
    
    assert(_d && _d->scene);
    size_t ret = 0;
    SceneNode::shared_ptr p;
    if (parent) {
        p = parent;
    } else {
        p = _d->scene;
    }
    if (p) {
        ret = p->count();
    }
//    std::cerr << ret;
    return ret;
}

SceneNode::shared_ptr Document::findItem(uint32_t iid) 
{
    TRACEFUNCTION("");
    //std::cerr << "Arg1: " << iid;
    SceneNode::shared_ptr p;
    return _d->scene->findByIID(iid);
}

void Document::addItem(const SceneNode::shared_ptr & node, 
                     const SceneNode::shared_ptr & parent)
{
    TRACEFUNCTION("");
//    std::cerr << "Arg1: " << node->iid() << " Arg2:" << parent;
    assert(_d && _d->scene);
    
    SceneNode::shared_ptr p;
    if (parent) {
        p = parent;
    }
    
    if (!p) {
        p = _d->scene;
    }
    if (p) {
        p->add(node);
    } else {
        std::cerr << "failed to add node.";
    }
}

static void
getCameraRecursive(const SceneNode::shared_ptr & scene,
                   CameraNode::shared_ptr & container)
{
    if (container) return;
    
    auto it = scene->iterator();
    while(it.hasNext()) {
        SceneNode::shared_ptr child = it.next();
        CameraNode::shared_ptr cam = std::dynamic_pointer_cast<CameraNode>(child);
        if (cam) {
            container = cam;
        } else {
            getCameraRecursive(child, container);
        }
    }
}

CameraNode::shared_ptr
Document::getCamera() const
{
    CameraNode::shared_ptr res;
    
    getCameraRecursive(_d->scene, res);
    
    return res;
}

Iterator<SceneNode> Document::sceneIterator()
{
    auto ptr = std::static_pointer_cast<Document>(shared_from_this());
    return Iterator<SceneNode>(new SceneIterator(ptr));
}

Iterator<SceneNode> Document::constSceneIterator() const
{
    auto ptr = std::static_pointer_cast<const Document>(shared_from_this());
    return Iterator<SceneNode>(new SceneIterator(ptr));
}

Iterator<ISurface> Document::surfaceIterator()
{
    auto ptr = std::static_pointer_cast<Document>(shared_from_this());
    return Iterator<ISurface>(new SurfaceIterator(ptr));
}


/// Iterator definition
SurfaceIterator::SurfaceIterator(Document::weak_ptr doc)
    :	_doc(doc)
{
    assert(!doc.expired());
}

SurfaceIterator::~SurfaceIterator()
{
}


IIterator<ISurface>* SurfaceIterator::clone() const
{
    SurfaceIterator *it = new SurfaceIterator(_doc);
    return it;
}

bool SurfaceIterator::hasNext() const
{
    NOT_IMPLEMENTED;
}

bool SurfaceIterator::hasPrevious() const
{
    NOT_IMPLEMENTED;
}

ISurface::shared_ptr SurfaceIterator::next()
{
    NOT_IMPLEMENTED;
}

ISurface::const_shared_ptr SurfaceIterator::next() const
{
    NOT_IMPLEMENTED;
}

ISurface::shared_ptr SurfaceIterator::previous()
{
    NOT_IMPLEMENTED;
}

ISurface::const_shared_ptr SurfaceIterator::previous() const
{
    NOT_IMPLEMENTED;
}

bool SurfaceIterator::seek(int /*pos*/, IteratorOrigin /*origin*/) const
{
    NOT_IMPLEMENTED;
}


SceneIterator::SceneIterator(const Document::const_weak_ptr & doc)
:	_doc(doc),
    _current(0)
{
    assert(!doc.expired());
    _scene = _doc->scene().lock();
}

SceneIterator::~SceneIterator()
{
    _doc = NULL;
    _scene.reset();
}

IIterator<SceneNode>* SceneIterator::clone() const
{
    SceneIterator *it = new SceneIterator(_doc);
    return it;
}

bool SceneIterator::hasNext() const
{

    return _current < _scene->count();
}

bool SceneIterator::hasPrevious() const
{
    NOT_IMPLEMENTED;
}

SceneNode::shared_ptr SceneIterator::next()
{
    SceneNode::weak_ptr n = _scene->item(_current);
    _current++;
    return n.lock();
}

SceneNode::const_shared_ptr SceneIterator::next() const
{
    SceneNode::weak_ptr n = _scene->item(_current);
    _current++;
    return n.lock();
}

SceneNode::shared_ptr SceneIterator::previous()
{
    NOT_IMPLEMENTED;
}

SceneNode::const_shared_ptr SceneIterator::previous() const
{
    NOT_IMPLEMENTED;
}

bool SceneIterator::seek(int /*pos*/, IteratorOrigin /*origin*/) const
{
    NOT_IMPLEMENTED;
}
