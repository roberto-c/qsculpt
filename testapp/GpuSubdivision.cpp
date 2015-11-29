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

#define __CL_ENABLE_EXCEPTIONS
#include <atomic>
#include <OpenCL/opencl.h>
#include "cl.hpp"
#include <math.h>
#include <Eigen/Core>
#include <map>
#include <unordered_map>
#include "GpuSubdivision.h"
#include "ClStlAllocator.h"

#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Mesh.h>
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/subdivision/SubdivisionRenderable.h>

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

static std::atomic_int NEXT_ID;

//typedef std::pair<Vertex::size_t, Vertex::size_t>     VtxPair;
//typedef std::unordered_map<Vertex::size_t, Vertex*>   VertexCollection;
//typedef std::unordered_map<Edge::size_t, Edge*>       EdgesCollection;
//typedef std::unordered_map<VtxPair, Edge::size_t>     VtxPairEdgeMap;
//typedef std::map<Face::size_t, Face*>                 FacesCollection;

namespace core
{
namespace subdivision {

    struct Vertex {
        float 	p[4];
        float 	c[4];
        float 	t[2];
        cl_uint edgeIid;
        cl_uint faceIid;
        
        Vertex() : p(), c(), t(), edgeIid(-1), faceIid(-1)
        {}
    };
    
    struct Edge {
        cl_uint	edgePairIid;
        cl_uint edgeNextIid;
        cl_uint vertexHeadIid;
        cl_uint faceIid;
        
        Edge() : edgePairIid(-1), edgeNextIid(-1), vertexHeadIid(-1), faceIid(-1)
        {}
    };
    
    struct Face {
        cl_uint edgeIid;
    };
};
};

using Vertex_t = core::subdivision::Vertex;
using Edge_t = core::subdivision::Edge;
using Face_t = core::subdivision::Face;
template<class T> using vector = std::vector<T, core::cl::allocator<T>>;

typedef vector<Vertex_t>  VertexCollection;
typedef vector<Edge_t>    EdgesCollection;
typedef vector<Face_t>    FacesCollection;

namespace core {
    struct clSurfaceList;
    
    struct GpuSubdivision::Impl : public IRenderable {
    /** Instance ID of the surface */
    ISurface::size_t _iid;
    
    VertexCollection    *_vertices;
    EdgesCollection     *_edges;
    FacesCollection     *_faces;
    
    Scene*          _scene;
    Color           _color;
    geometry::AABB  _boundingBox;
    bool            _selected;
    int             _callListId;
    bool            _genereateCallList;
    int             _currentResolutionLevel;
    bool            _hasChanged;
        
    static bool 				oclInitialized;
    static ::cl::Program  		program;
    static ::cl::Kernel			krnInit;
    static ::cl::Kernel 			krnSubdivideEdges;
    static ::cl::Kernel			krnSubdivideAddFaces;
    static ::cl::Kernel			krnSubdivideAdjustPos;
    
    Impl(GpuSubdivision * surface) :     _vertices(NULL),
    _edges(NULL),
    _faces(NULL),
    _scene(NULL),
    _color(1.f,1.f,1.f),
    _selected(false),
    _callListId(0),
    _genereateCallList(true),
    _currentResolutionLevel(0),
    _hasChanged(true)
    {
    }
    
    void 		subdivide(GpuSubdivision * surf);
    
    static int 	initialize_ocl(void);
        
    virtual void render(const RenderState * state) const;
};
    
bool 				GpuSubdivision::Impl::oclInitialized;
::cl::Program  		GpuSubdivision::Impl::program;
::cl::Kernel		GpuSubdivision::Impl::krnInit;
::cl::Kernel 		GpuSubdivision::Impl::krnSubdivideEdges;
::cl::Kernel		GpuSubdivision::Impl::krnSubdivideAddFaces;
::cl::Kernel		GpuSubdivision::Impl::krnSubdivideAdjustPos;

// Iterator classes declarations
class GpuSubdivision::VertexIterator : public IIterator<VertexHandle>
{
    friend class GpuSubdivision;
public:
    typedef VertexCollection::iterator iterator;
    
    const GpuSubdivision*  _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(const GpuSubdivision* surface, int level = -1);
    
public:
    /**
     *
     */
    IIterator<VertexHandle>* clone() const;
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    bool hasNext() const;
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    bool hasPrevious() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class GpuSubdivision::FaceIterator : public IIterator<FaceHandle>
{
    friend class GpuSubdivision;
    
    typedef FacesCollection::iterator iterator;
    
    char tmp[4];
    const GpuSubdivision*  _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(const GpuSubdivision* v, int level = -1);
    
public:
    /**
     *
     */
    IIterator<FaceHandle>* clone() const;
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const;
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    bool hasPrevious() const;
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class GpuSubdivision::EdgeIterator : public IIterator<EdgeHandle>
{
    friend class GpuSubdivision;
    
    typedef EdgesCollection::iterator iterator;
    
    const GpuSubdivision*  surface_;
    int                 level_;
    mutable iterator    index_;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator(const GpuSubdivision* s, int level = -1)
    : surface_(s), level_(level)
    {
        index_ = surface_->_d->_edges->begin();
    }
    
public:
    /**
     *
     */
    IIterator<EdgeHandle>* clone() const {
        EdgeIterator *it = new EdgeIterator(surface_, level_);
        it->index_ = index_;
        return it;
    }
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const {
        return (index_ != surface_->_d->_edges->end());
    }
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    bool hasPrevious() const {
        NOT_IMPLEMENTED
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr next() {
        NOT_IMPLEMENTED
        Edge * e = nullptr;
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const {
        NOT_IMPLEMENTED
        Edge * e = nullptr;
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext() {
        NOT_IMPLEMENTED
        return nullptr;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const {
        NOT_IMPLEMENTED
        return nullptr;
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous() {
        NOT_IMPLEMENTED
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const shared_ptr previous() const {
        NOT_IMPLEMENTED
    }
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const {
        NOT_IMPLEMENTED
    }
};
    
void GpuSubdivision::Impl::render(const RenderState * state) const
{
    
}

GpuSubdivision::GpuSubdivision()
:   ISurface(),
_d(new Impl(this))
{
    _d->_iid = NEXT_ID.fetch_add(1);
    initPoints();
    //updateBoundingBox();
}

GpuSubdivision::GpuSubdivision(const Mesh * mesh)
: ISurface(),
_d(new Impl(this))
{
    convertFromMesh(mesh);
}

GpuSubdivision::~GpuSubdivision()
{
    _d->_vertices = NULL;
    _d->_edges = NULL;
    _d->_faces = NULL;
}

ISurface::size_t GpuSubdivision::iid() const
{
    return _d->_iid;
}

void GpuSubdivision::initPoints()
{
    lock();
    _d->_vertices = new VertexCollection;
    _d->_edges = new EdgesCollection;
    _d->_faces = new FacesCollection;
    
    _d->_vertices->reserve(50000);
    _d->_edges->reserve(50000);
    
    unlock();
}

bool GpuSubdivision::hasChanged() {
    return _d->_hasChanged;
}

void GpuSubdivision::setChanged(bool val) {
    _d->_hasChanged = val;
    if (val) {
        meshChanged(this);
    }
}

std::vector<VertexHandle::size_t> GpuSubdivision::selectedPoints() const {
    return std::vector<VertexHandle::size_t>();
}

void GpuSubdivision::setSelectedPoints(const std::vector<VertexHandle::size_t>& indicesArray) {
    //_d->_selectedPoints = indicesArray;
}

void GpuSubdivision::setScene(Scene* scene)
{
    _d->_scene = scene;
}

Scene* GpuSubdivision::scene() const
{
    return _d->_scene;
}

const IRenderable* GpuSubdivision::renderable() const
{
    return _d.get();
}

const geometry::AABB& GpuSubdivision::boundingBox() const
{
    return _d->_boundingBox;
}

void GpuSubdivision::setColor(const Color& color)
{
    _d->_color = color;
    auto it = vertexIterator();
    while (it.hasNext()) {
        static_cast<Vertex*>(it.next())->color() = color;
    }
}

Color GpuSubdivision::color() const
{
    return _d->_color;
}

void GpuSubdivision::setSelected(bool val)
{
    _d->_selected = val;
}

bool GpuSubdivision::isSelected() const
{
    return _d->_selected;
}

VertexHandle::size_t GpuSubdivision::addVertex(const Point3& point)
{
    NOT_IMPLEMENTED
    return -1;
}

VertexHandle::size_t GpuSubdivision::addVertex(VertexHandle* v)
{
    NOT_IMPLEMENTED
    return -1;
}

void GpuSubdivision::removeVertex(VertexHandle::size_t iid)
{
    //TODO: need to mark as free space
}

VertexHandle* GpuSubdivision::vertex(VertexHandle::size_t iid)
{
    NOT_IMPLEMENTED
    return nullptr;
}

const VertexHandle* GpuSubdivision::vertex(size_t iid) const
{
    NOT_IMPLEMENTED
    return nullptr;
}

VertexHandle::size_t GpuSubdivision::numVertices() const
{
    return static_cast<VertexHandle::size_t>(_d->_vertices->size());
}

EdgeHandle::size_t GpuSubdivision::addEdge(const EdgeHandle& e)
{
    const Edge * edge = static_cast<const Edge*>(&e);
    return addEdge(edge->pair()->head(), edge->head());
}

EdgeHandle::size_t GpuSubdivision::addEdge(VertexHandle::size_t v1, VertexHandle::size_t v2)
{
    NOT_IMPLEMENTED
    return -1;
//    return addEdge(_d->_vertices->at(v1), _d->_vertices->at(v2));
}

EdgeHandle::size_t GpuSubdivision::addEdge(VertexHandle* tail, VertexHandle* head)
{
    assert(tail && head);
    
    EdgeHandle::size_t iid = -1;
    
    return iid;
}

EdgeHandle::size_t GpuSubdivision::edge(VertexHandle::size_t iidVtxTail, VertexHandle::size_t iidVtxHead) const
{
    assert(iidVtxTail > 0 && iidVtxHead > 0);
    
    size_t iid = 0;
    
    return iid;
}

EdgeHandle * GpuSubdivision::edge(EdgeHandle::size_t iidEdge)
{
    NOT_IMPLEMENTED
    return nullptr;
//    return _d->_edges->at(iidEdge);
}

const EdgeHandle * GpuSubdivision::edge(EdgeHandle::size_t iidEdge) const
{
    NOT_IMPLEMENTED
    return nullptr;
//    return _d->_edges->at(iidEdge);
}

EdgeHandle::size_t GpuSubdivision::numEdges() const
{
    return static_cast<EdgeHandle::size_t>(_d->_edges->size());
}

FaceHandle::size_t GpuSubdivision::addFace(const std::vector<VertexHandle::size_t>& vertexIndexList)
{
    //NOT_IMPLEMENTED
    std::vector<Edge*> edges;
    auto size = vertexIndexList.size();
    if (size < 3) {
        std::cerr << "addFace: not enough vertices: " << size << std::endl;
        return -1;
    }
    FaceHandle::size_t iid = -1;
    return iid;
}

void GpuSubdivision::replaceFace(FaceHandle::size_t /*faceIndex*/, const std::vector<VertexHandle::size_t>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void GpuSubdivision::removeFace( FaceHandle::size_t iid)
{
    NOT_IMPLEMENTED
}

FaceHandle* GpuSubdivision::face(size_t iid)
{
    NOT_IMPLEMENTED
    return nullptr;
}

FaceHandle::size_t GpuSubdivision::numFaces() const
{
    assert( _d->_faces!= NULL );
    return static_cast<FaceHandle::size_t>(_d->_faces->size());
}


FaceHandle::size_t GpuSubdivision::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

VertexHandle::size_t GpuSubdivision::getClosestPointAtPoint(const Point3 & p) const
{
    float d = MAXFLOAT;
    Vertex::shared_ptr vtx = nullptr, tmpVtx = nullptr;
    Point3 v;
    Iterator<VertexHandle> it = constVertexIterator();
    while (it.hasNext()) {
        tmpVtx = static_cast<Vertex*>(it.next());
        v = tmpVtx->position() - p;
        if (v.squaredNorm() < d) {
            d = v.squaredNorm();
            vtx = tmpVtx;
        }
    }
    
    return vtx ? vtx->iid() : 0;
}

std::vector<VertexHandle::size_t> GpuSubdivision::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //std::cerr << "GpuSubdivision::getPointsInRadius()";
    std::vector<VertexHandle::size_t> results;
    
    NOT_IMPLEMENTED
    
    return results;
}

void GpuSubdivision::lock() const
{
    // lock access
    // acquire gpu memory pointers and update container with valid pointers
    NOT_IMPLEMENTED
    //_mutex.lock();
}

void GpuSubdivision::unlock() const
{
    // invalidate pointers/submit data to gpu
    // release lock
    NOT_IMPLEMENTED
    //_mutex.unlock();
}

void GpuSubdivision::addResolutionLevel()
{
    //TODO: Implement addResolutionLevel
    //NOT_IMPLEMENTED
    _d->subdivide(this);
    //	_d->subdivide(this);
}

void GpuSubdivision::removeResolutionLevel(int level)
{
    // TODO: Implement removeResolutionLevel
    NOT_IMPLEMENTED
}

void GpuSubdivision::setWorkingResolutionLevel(int level)
{
    // TODO: Implement setWorkingResolutionLevel
    NOT_IMPLEMENTED
}

int GpuSubdivision::workingResolutionLevel()
{
    // TODO: Implement getWorkingResolutionLevel
    NOT_IMPLEMENTED
    return _d->_currentResolutionLevel;
}

void GpuSubdivision::adjustPointNormal(size_t /*index*/)
{
    NOT_IMPLEMENTED
}

Iterator<VertexHandle> GpuSubdivision::vertexIterator()
{
    return Iterator<VertexHandle>(new VertexIterator(this) );
}

Iterator<VertexHandle> GpuSubdivision::constVertexIterator() const
{
    return Iterator<VertexHandle>(new VertexIterator(this) );
}

Iterator<FaceHandle> GpuSubdivision::faceIterator()
{
    return Iterator<FaceHandle>(new FaceIterator(this));
}

Iterator<FaceHandle> GpuSubdivision::constFaceIterator() const
{
    return Iterator<FaceHandle>(new FaceIterator(this));
}

Iterator<VertexHandle> GpuSubdivision::vertexIterator(int level)
{
    return Iterator<VertexHandle>(new VertexIterator(this, level) );
}

Iterator<VertexHandle> GpuSubdivision::constVertexIterator(int level) const
{
    return Iterator<VertexHandle>(new VertexIterator(this, level) );
}

Iterator<FaceHandle> GpuSubdivision::faceIterator(int level)
{
    return Iterator<FaceHandle>(new FaceIterator(this, level));
}

Iterator<FaceHandle> GpuSubdivision::constFaceIterator(int level) const
{
    return Iterator<FaceHandle>(new FaceIterator(this, level));
}

Iterator<EdgeHandle> GpuSubdivision::edgeIterator()
{
    return Iterator<EdgeHandle>(new EdgeIterator(this));
}

Iterator<EdgeHandle> GpuSubdivision::constEdgeIterator() const
{
    return Iterator<EdgeHandle>(new EdgeIterator(this));
}

Mesh* GpuSubdivision::convertToMesh(int level)
{
    Mesh * mesh = new ::Mesh;
    
    return mesh;
}

/**
 * Uses mesh as base mesh for this subdivision surface.
 *
 * @param mesh mesh to convert from.
 */
void GpuSubdivision::convertFromMesh(const Mesh* mesh)
{
    
}


//////////////////////////////////////////////////////////////////////////////
// Inner classes implementation
// GpuSubdivision::VertexIterator
GpuSubdivision::VertexIterator::VertexIterator(const GpuSubdivision* surface, int level)
:	_surface(surface),
_level(level)
{
    _index = _surface->_d->_vertices->begin();
}


IIterator<VertexHandle>* GpuSubdivision::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool GpuSubdivision::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n >0 && _index != _surface->_d->_vertices->end();
}

bool GpuSubdivision::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n > 0 &&
    (_index == _surface->_d->_vertices->end() ||
     _index != _surface->_d->_vertices->begin());
}

GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::next()
{
    NOT_IMPLEMENTED
    return nullptr;
}

const GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::next() const
{
    NOT_IMPLEMENTED
    return nullptr;
}

GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::peekNext() {
    NOT_IMPLEMENTED
    return nullptr;
}

const GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::peekNext() const {
    NOT_IMPLEMENTED
    return nullptr;
}

GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::previous()
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

const GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

bool GpuSubdivision::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    switch(origin)
    {
        case Iter_Current:
            // nothing
            break;
        case Iter_End:
            _index = _surface->_d->_vertices->end();
            break;
            
        case Iter_Start:
        default:
            _index = _surface->_d->_vertices->begin();
            break;
    }
    
    if (pos > 0) {
        while(--pos && _index != _surface->_d->_vertices->end()) {
            ++_index;
        }
        if (_index == _surface->_d->_vertices->end())
            return false;
    } else if (pos < 0 ) {
        std::cerr << "GpuSubdivision::VertexIterator::seek: not supported" << std::endl;
        return false;
        //        while(++pos && _index != _surface->_d->_vertices->end()) {
        //            --_index;
        //        }
        //        if (_index == _surface->_d->_vertices->end())
        //            return false;
    }
    return true;
}

//GpuSubdivision::FaceIterator
GpuSubdivision::FaceIterator::FaceIterator(const GpuSubdivision* surface, int level)
:	_surface(surface),
_level(level)
{
    assert(surface);
    this->tmp[0] = 'a';
    this->tmp[1] = 'b';
    _index = _surface->_d->_faces->begin();
}

IIterator<FaceHandle>* GpuSubdivision::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool GpuSubdivision::FaceIterator::hasNext() const
{
    
    size_t n = _surface->numFaces();
    
    return n > 0 && _index != _surface->_d->_faces->end();
}

bool GpuSubdivision::FaceIterator::hasPrevious() const
{
    size_t n = _surface->numFaces();
    
    return n > 0 &&
    (_index == _surface->_d->_faces->end() ||
     _index != _surface->_d->_faces->begin());
}

GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::next()
{
    NOT_IMPLEMENTED
    return nullptr;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::next() const
{
    NOT_IMPLEMENTED
    return nullptr;
}

GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::peekNext() {
    NOT_IMPLEMENTED
    return nullptr;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::peekNext() const {
    NOT_IMPLEMENTED
    return nullptr;
}


GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::previous()
{
    NOT_IMPLEMENTED
    return nullptr;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::previous() const
{
    NOT_IMPLEMENTED
    return nullptr;
}

bool GpuSubdivision::FaceIterator::seek(int pos, IteratorOrigin origin) const
{
    switch(origin)
    {
        case Iter_Current:
            //nothing
            break;
        case Iter_End:
            _index = _surface->_d->_faces->end();
            break;
            
        case Iter_Start:
        default:
            _index = _surface->_d->_faces->begin();
            break;
    }
    
    if (pos > 0) {
        while(--pos && _index != _surface->_d->_faces->end()) {
            ++_index;
        }
        if (_index == _surface->_d->_faces->end())
            return false;
    } else if (pos < 0 ) {
        while(++pos && _index != _surface->_d->_faces->end()) {
            --_index;
        }
        if (_index == _surface->_d->_faces->end())
            return false;
    }
    return true;
}

void GpuSubdivision::printMemoryInfo() const
{
    Vertex::size_t sizeVertex, nVertex;
    Edge::size_t sizeEdge, nEdge;
    Face::size_t sizeFace, nFace;
    
    sizeVertex = sizeof(Vertex);
    sizeFace = sizeof(Face);
    sizeEdge = sizeof(Edge);
    nVertex = numVertices();
    nFace = numFaces();
    nEdge = numEdges();
    
    std::cerr << "Size of vertex: " << sizeVertex;
    std::cerr << "Size of Face: " << sizeFace;
    std::cerr << "Size of Edge: " << sizeEdge;
    std::cerr << "Num verts: " << nVertex;
    std::cerr << "Num facess: " << nFace;
    std::cerr << "Num edges: " << nEdge;
    
    std::cerr << "Size of vertex hash: " << sizeof(*_d->_vertices);
    
    std::cerr << "Mem usage: " << (sizeVertex * nVertex)
    + (sizeEdge * nEdge)
    + (sizeFace * nFace)
    + sizeof(*_d->_vertices);
}




int
GpuSubdivision::Impl::initialize_ocl(void)
{
    cl_int err = CL_SUCCESS;
    
    try {
        CLManager * oclManager = CLManager::instance();
        
        ResourcesManager mgr;
        std::string path = mgr.findResourcePath("Subdivision", "cl");
        std::string kernelSource = opencl::loadFromFile(path);
        ::cl::Program::Sources source(1,
                                    std::make_pair(kernelSource.c_str(),kernelSource.length()));
        program = ::cl::Program(oclManager->context(), source);
        program.build(oclManager->devices());
        
        krnInit = ::cl::Kernel(program,"surface_init",&err);
        krnSubdivideEdges = ::cl::Kernel(program, "subdivide_edges_midpoint", &err);
        std::cout << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: " << krnSubdivideEdges.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(NULL) << std::endl;
        krnSubdivideAddFaces = ::cl::Kernel(program, "subdivide_add_faces", &err);
        krnSubdivideAdjustPos = ::cl::Kernel(program,"subdivide_vertices_adjust_pos", &err);
        std::cout << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: " << krnSubdivideAdjustPos.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(NULL) << std::endl;
    }
    catch (::cl::Error err) {
        std::cerr
        << "ERROR: "
        << err.what()
        << "("
        << err.err()
        << ")"
        << std::endl;
    }
    
    return err;
}


struct clVertex {
    cl_float4 	p;	// position
    cl_float4 	n;	// normal
    cl_uint		e;	// edge index
};

struct clEdge {
    cl_uint	head;	// index to vertex array
    cl_uint pair; 	// index to edge array
    cl_uint face;	// index to a face
    cl_uint next;	// next half-edge
    //	cl_uint midpoint; // index to mid vertex point
};

struct clFace {
    cl_uint 	num_edges;	// number of edges
    cl_uint 	edge;		// first edge
    cl_uint		vertex;		// vertex index in face
};

struct clSurfaceList {
    uint32_t 					num_polygons;
    std::vector<clVertex>	vertex;
    std::vector<clEdge>		edge;
    std::vector<clFace>		face;
};

struct clSurface {
    cl_uint					num_vertices;
    cl_uint					num_edges;
    cl_uint					num_faces;
    cl_ulong  				v;
    cl_ulong				e;
    cl_ulong				f;
    cl_uint					user[4];
};

void GpuSubdivision::Impl::subdivide(GpuSubdivision * s)
{
    if (!oclInitialized) {
        initialize_ocl();
        oclInitialized = true;
    }
}
    
}; /* End namespace*/
