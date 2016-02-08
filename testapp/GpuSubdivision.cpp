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
#include "stable.h"

#include <atomic>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <math.h>
#include <Eigen/Core>
#include <map>
#include <unordered_map>
#include "GpuSubdivision.h"
#include "GpuSubdivision_private.h"

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Mesh.h>
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>
#include <PlastilinaCore/RenderState.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/subdivision/Box.h>
#include "GpuSubdivisionRenderable.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

static std::atomic_int NEXT_ID;
const core::gpusubdivision::Edge core::gpusubdivision::Edge::EDGE_END;
const core::gpusubdivision::Vertex core::gpusubdivision::Vertex::VERTEX_END;

namespace
{
class FaceVertexIterator : public IIterator<VertexHandle>
{
    const core::GpuSubdivision                  *surface;
    const core::gpusubdivision::Face            *face;
    mutable const core::gpusubdivision::Edge    *currentEdge;
public:
    FaceVertexIterator(const core::GpuSubdivision *surface, const Face *face, unsigned int flags = 0)
    : surface(surface), face(face), currentEdge(nullptr)
    {
        currentEdge = static_cast<const Edge*>(surface->edge(face->edge));
    }
    
    IIterator<VertexHandle>* clone() const;
    
    bool hasNext() const;
    
    bool hasPrevious() const;
    
    shared_ptr next();
    
    const_shared_ptr next() const;
    
    shared_ptr peekNext();
    
    const_shared_ptr peekNext() const ;
    
    shared_ptr previous();
    
    const_shared_ptr previous() const;
    
    bool seek(int pos, IteratorOrigin origin) const ;

    virtual bool operator==(const IIterator & rhs)
    {
        if (this == &rhs)
            return true;
        const FaceVertexIterator * r = static_cast<const FaceVertexIterator*>(&rhs);
        return this->surface == r->surface
            && this->face == r->face
            && this->currentEdge == r->currentEdge;
    }

    friend bool operator==(const FaceVertexIterator &lhs, const FaceVertexIterator& rhs)
    {
        return lhs.surface == rhs.surface
            && lhs.face == rhs.face
            && lhs.currentEdge == rhs.currentEdge;
    }
    friend bool operator!=(const FaceVertexIterator &lhs, const FaceVertexIterator& rhs)
    {
        return !(lhs == rhs);
    }
};
};

namespace core
{
namespace gpusubdivision
{

Iterator<VertexHandle>
Face::vertexIterator(const ISurface* surface)
{
    const GpuSubdivision *gpusurf = dynamic_cast<const GpuSubdivision*>(surface);
    return Iterator<VertexHandle>(new FaceVertexIterator(gpusurf, this));
}

Iterator<VertexHandle>
Face::constVertexIterator(const ISurface* surface) const
{
    const GpuSubdivision *gpusurf = dynamic_cast<const GpuSubdivision*>(surface);
    return Iterator<VertexHandle>(new FaceVertexIterator(gpusurf, this));
}

Iterator<EdgeHandle>
Face::edgeIterator(ISurface* surface)
{
    NOT_IMPLEMENTED
}

Iterator<EdgeHandle>
Face::constEdgeIterator(ISurface* surface) const
{
    NOT_IMPLEMENTED
}

Vertex::Vertex()
    : VertexHandle(VertexHandleType::GPUSUBDIVISION)
    , device::Vertex()
{
    TRACE(debug) << "Vertex()";
}

Vertex::Vertex(const Vertex& cpy)
    : VertexHandle(cpy)
    , device::Vertex(cpy)
{
    TRACE(debug) << "Vertex(const Vertex& cpy)";
}

Vertex::Vertex(Vertex&& cpy)
    : VertexHandle(cpy)
    , device::Vertex(cpy)
{
    TRACE(debug) << "Vertex(Vertex&& cpy)";
}

}
}

namespace core {
GpuSubdivision::Impl::Impl(GpuSubdivision * surface) 
    : _surface(surface),
    _vertices(NULL),
    _edges(NULL),
    _faces(NULL),
    _triangleOutput(NULL),
    _lock(_mutex, std::defer_lock),
    _scene(NULL),
    _color(1.f,1.f,1.f),
    _selected(false),
    _callListId(0),
    _genereateCallList(true),
    _currentResolutionLevel(0),
    _hasChanged(true),
    _renderable(new GpuSubdivisionRenderable(surface))
    {
        _dataBuffer = std::unique_ptr<VertexBuffer>(BOManager::getInstance()->createVBO("GPUSUBDIVISION", surface));
    }

GpuSubdivision::Impl::~Impl()
{
    BOManager::getInstance()->destroyAllMeshBO(_surface);
}
    
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
    const_shared_ptr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const_shared_ptr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const_shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;

    virtual bool operator==(const IIterator & rhs)
    {
        if (this == &rhs)
            return true;
        const VertexIterator * r = static_cast<const VertexIterator*>(&rhs);
        return this->_surface == r->_surface
            && this->_level == r->_level
            && this->_index == r->_index;
    }

    friend bool operator==(const VertexIterator &lhs, const VertexIterator& rhs)
    {
        return lhs._surface == rhs._surface
            && lhs._level == rhs._level
            && lhs._index == rhs._index;
    }
    friend bool operator!=(const VertexIterator &lhs, const VertexIterator& rhs)
    {
        return !(lhs == rhs);
    }
};

class GpuSubdivision::FaceIterator : public IIterator<FaceHandle>
{
    friend class GpuSubdivision;
    
    typedef FacesCollection::iterator iterator;
    
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
    const_shared_ptr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const_shared_ptr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const_shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;

    virtual bool operator==(const IIterator & rhs)
    {
        if (this == &rhs)
            return true;
        const FaceIterator * r = static_cast<const FaceIterator*>(&rhs);
        return this->_surface == r->_surface
            && this->_level == r->_level
            && this->_index == r->_index;
    }

    friend bool operator==(const FaceIterator &lhs, const FaceIterator& rhs)
    {
        return lhs._surface == rhs._surface
            && lhs._level == rhs._level
            && lhs._index == rhs._index;
    }
    friend bool operator!=(const FaceIterator &lhs, const FaceIterator& rhs)
    {
        return !(lhs == rhs);
    }
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
    const_shared_ptr next() const {
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
    const_shared_ptr peekNext() const {
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
    const_shared_ptr previous() const {
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
    delete _d->_vertices;
    delete _d->_edges;
    delete _d->_faces;
    delete _d->_triangleOutput;
}

ISurface::size_t GpuSubdivision::iid() const
{
    return _d->_iid;
}

void GpuSubdivision::initPoints()
{
    lock();
    CLManager * oclManager = CLManager::instance();
    
    ::cl::Context clctx = oclManager->context();
    ::cl::CommandQueue clqueue= oclManager->commandQueue();
    VertexCollection::allocator_type gpu_allocator(clctx,clqueue);
    _d->_vertices = new VertexCollection(gpu_allocator);
    _d->_edges = new EdgesCollection(gpu_allocator);
    _d->_faces = new FacesCollection(gpu_allocator);
    GlVertexCollection::allocator_type glvertex_allocator(clctx,clqueue);
    _d->_triangleOutput = new GlVertexCollection(glvertex_allocator);
    
    _d->_vertices->reserve(500);
    _d->_edges->reserve(500);
    _d->_faces->reserve(500);
    _d->_triangleOutput->reserve(500);

    using Eigen::Vector4f;
    std::vector<Vector4f> buffer;
    buffer.resize(12);
    buffer[0] = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    buffer[1] = Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
    buffer[2] = Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    buffer[3] = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    buffer[4] = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
    buffer[5] = Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
    buffer[6] = Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    buffer[7] = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    buffer[8] = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
    buffer[9] = Vector4f(0.0f, 0.0f, 1.0f, 0.0f);
    buffer[10] = Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    buffer[11] = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    _d->_dataBuffer->setBufferData(buffer.data(), buffer.size()*sizeof(Vector4f));

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
    assert(_d != nullptr && _d->_lock.owns_lock());
    _d->_color = color;
    auto it = vertexIterator();
    while (it.hasNext()) {
        static_cast<Vertex*>(it.next())->c = utils::convert_to<cl_float4>(color.data());
    }
}

Color GpuSubdivision::color() const
{
    assert(_d != nullptr && _d->_lock.owns_lock());
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
    assert(_d != nullptr && _d->_lock.owns_lock());
    _d->_vertices->push_back(Vertex());
    Vertex * v = &_d->_vertices->back();
    v->setIid(_d->_vertices->size());
    v->p.s[0] = point[0];
    v->p.s[1] = point[1];
    v->p.s[2] = point[2];
    v->p.s[3] = 1;
    return v->iid();
}

VertexHandle::size_t GpuSubdivision::addVertex(VertexHandle* v)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    _d->_vertices->push_back(Vertex());
    Vertex * v1 = &_d->_vertices->back();
    v1->setIid(_d->_vertices->size());
    if (v1->type() == v->type()) {
        auto vn = static_cast<Vertex*>(v);
        v1->p = vn->p;
    }
    return v1->iid();
}

void GpuSubdivision::removeVertex(VertexHandle::size_t iid)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    //TODO: need to mark as free space
}

VertexHandle* GpuSubdivision::vertex(VertexHandle::size_t iid)
{
    return &_d->_vertices->at(iid - 1);
}

const VertexHandle* GpuSubdivision::vertex(size_t iid) const
{
    return iid == 0 ? &Vertex::VERTEX_END : &_d->_vertices->at(iid - 1);
}

VertexHandle::size_t GpuSubdivision::numVertices() const
{
    return static_cast<VertexHandle::size_t>(_d->_vertices->size());
}

EdgeHandle::size_t GpuSubdivision::addEdge(const EdgeHandle& e)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    const Edge * edge = static_cast<const Edge*>(&e);
    const Edge * edge_pair = static_cast<const Edge*>(this->edge(edge->pair));
    Vertex::size_t pair_head = edge_pair->head;
    return addEdge(pair_head, edge->head);
}

EdgeHandle::size_t GpuSubdivision::addEdge(VertexHandle::size_t v1, VertexHandle::size_t v2)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    assert(_d && _d->_vertices && v1 <= _d->_vertices->size() && v2 <= _d->_vertices->size());

    Edge::size_t iid = edge(v2, v1);
    if (iid != 0)
        throw std::runtime_error("Edge already exists");

    _d->_edges->push_back(Edge());
    Edge * edge1 = &_d->_edges->back();
    edge1->setIid(_d->_edges->size());
    _d->_edges->push_back(Edge());
    Edge * edge2 = &_d->_edges->back();
    edge2->setIid(_d->_edges->size());

    VertexPairKey vtx;
    vtx.first = v2;
    vtx.second = v1;
    _d->_vertexToEdge[vtx] = edge1->iid();
    vtx.first = v1;
    vtx.second = v2;
    _d->_vertexToEdge[vtx] = edge2->iid();

    edge1->head = v2;
    edge1->pair = edge2->iid();
    edge1->next = 0;
    edge2->head = v1;
    edge2->pair = edge1->iid();
    edge2->next = 0;

    return edge1->iid();
}

EdgeHandle::size_t GpuSubdivision::addEdge(VertexHandle* tail, VertexHandle* head)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    assert(tail && head);
    
    return addEdge(tail->iid(), head->iid());
}

EdgeHandle::size_t GpuSubdivision::edge(VertexHandle::size_t iidVtxTail, VertexHandle::size_t iidVtxHead) const
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    assert(iidVtxTail > 0 && iidVtxHead > 0);
    VertexPairKey vtx;
    vtx.first = iidVtxHead;
    vtx.second = iidVtxTail;

    auto it = _d->_vertexToEdge.find(vtx);
    if (it == _d->_vertexToEdge.end())
    {
        TRACE(error) << "Edge not found";
        return 0;
    }
    return (*it).second;
}

EdgeHandle * GpuSubdivision::edge(EdgeHandle::size_t iidEdge)
{
    assert(iidEdge > 0 && _d && _d->_edges && iidEdge <= _d->_edges->size());
    return &_d->_edges->at(iidEdge - 1);
}

const EdgeHandle * GpuSubdivision::edge(EdgeHandle::size_t iidEdge) const
{
    assert( _d && _d->_edges && iidEdge <= _d->_edges->size());
    return iidEdge == 0 ? &Edge::EDGE_END : &_d->_edges->at(iidEdge - 1);
}

EdgeHandle::size_t GpuSubdivision::numEdges() const
{
    assert(_d && _d->_edges);
    return static_cast<EdgeHandle::size_t>(_d->_edges->size());
}

FaceHandle::size_t GpuSubdivision::addFace(const std::vector<VertexHandle::size_t>& vertexIndexList)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    assert(_d && _d->_edges && _d->_faces);
    std::vector<Edge::size_t> edges;
    auto size = vertexIndexList.size();
    if (size < 3) {
        TRACE(error) << "addFace: not enough vertices: " << size;
        return -1;
    }
    for (decltype(size) i = 0; i < size; ++i) {
        Edge::size_t iid = edge(vertexIndexList[i], vertexIndexList[(i + 1) % size]);
        if (iid == 0)
            iid = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
        assert(iid > 0);
        edges.push_back(iid);
    }
    _d->_faces->push_back(Face());
    Face *f = &_d->_faces->back();
    f->setIid(_d->_faces->size());
    f->edge = edges[0];
    f->vertex = vertexIndexList[0];
    for (decltype(size) i = 0; i < size; ++i) {
        Edge* e_i = static_cast<Edge*>(edge(edges[i]));
        Edge* e_ii = static_cast<Edge*>(edge(edges[(i+1)%size]));
        e_i->next = e_ii->iid();
        e_i->face = f->iid();
    }
    static_cast<Edge*>(edge(edges[size-1]))->next = Edge::EDGE_END.iid();
    return f->iid();
}

void GpuSubdivision::replaceFace(FaceHandle::size_t /*faceIndex*/, const std::vector<VertexHandle::size_t>& /*vertexIndexList*/)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void GpuSubdivision::removeFace( FaceHandle::size_t iid)
{
    assert(_d != nullptr && _d->_lock.owns_lock());
    NOT_IMPLEMENTED
}

FaceHandle* GpuSubdivision::face(size_t iid)
{
    assert(iid > 0 && _d && _d->_faces && _d->_faces->size() >= iid);
    return &_d->_faces->at(iid - 1);
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
    float d = std::numeric_limits<float>::max();
    Vertex::shared_ptr vtx = nullptr, tmpVtx = nullptr;
    Point3 v;
    Iterator<VertexHandle> it = constVertexIterator();
    while (it.hasNext()) {
        tmpVtx = static_cast<Vertex*>(it.next());
        v = utils::convert_to<Point3>(tmpVtx->p);
        v = v - p;
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
    _d->_lock.lock();
    if (_d->_triangleOutput) core::cl::lock_container(*(_d->_triangleOutput));
    if (_d->_vertices) core::cl::lock_container(*(_d->_vertices));
    if (_d->_edges) core::cl::lock_container(*(_d->_edges));
    if (_d->_faces) core::cl::lock_container(*(_d->_faces));
}

void GpuSubdivision::unlock() const
{
    // invalidate pointers/submit data to gpu
    // release lock
    if (_d->_faces) core::cl::unlock_container(*(_d->_faces));
    if (_d->_edges) core::cl::unlock_container(*(_d->_edges));
    if (_d->_vertices) core::cl::unlock_container(*(_d->_vertices));
    // Resize gl buffer if available
    if (_d->_triangleOutput)
    {
        // Resize triangles output to hold the number of faces
        _d->_triangleOutput->resize(_d->_faces->size() * 6);
        core::cl::unlock_container(*(_d->_triangleOutput));
    }
    _d->_lock.unlock();
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
GpuSubdivision::VertexIterator::VertexIterator(
    const GpuSubdivision* surface
    ,int level)
: _surface(surface)
, _level(level)
{
    _index = _surface->_d->_vertices->begin();
}


IIterator<VertexHandle>*
GpuSubdivision::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool
GpuSubdivision::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n >0 && _index != _surface->_d->_vertices->end();
}

bool
GpuSubdivision::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n > 0 &&
    (_index == _surface->_d->_vertices->end() ||
     _index != _surface->_d->_vertices->begin());
}

GpuSubdivision::VertexIterator::shared_ptr
GpuSubdivision::VertexIterator::next()
{
    auto *v = &*_index;
    ++_index;
    return v;
}

GpuSubdivision::VertexIterator::const_shared_ptr
GpuSubdivision::VertexIterator::next() const
{
    auto *v = &*_index;
    ++_index;
    return v;
}

GpuSubdivision::VertexIterator::shared_ptr
GpuSubdivision::VertexIterator::peekNext()
{
    assert(hasNext());
    auto *v = &*_index;
    return v;
}

GpuSubdivision::VertexIterator::const_shared_ptr
GpuSubdivision::VertexIterator::peekNext() const {
    assert(hasNext());
    auto *v = &*_index;
    return v;
}

GpuSubdivision::VertexIterator::shared_ptr
GpuSubdivision::VertexIterator::previous()
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

GpuSubdivision::VertexIterator::const_shared_ptr
GpuSubdivision::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

bool
GpuSubdivision::VertexIterator::seek(int pos, IteratorOrigin origin) const
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
        _index += pos;
        if (_index == _surface->_d->_vertices->end())
            return false;
    } else if (pos < 0 ) {
        std::cerr << "GpuSubdivision::VertexIterator::seek: not supported" << std::endl;
        NOT_IMPLEMENTED
        return false;
    }
    return true;
}

//GpuSubdivision::FaceIterator
GpuSubdivision::FaceIterator::FaceIterator(
    const GpuSubdivision* surface
    ,int level)
: _surface(surface)
, _level(level)
{
    assert(surface);
    _index = _surface->_d->_faces->begin();
}

IIterator<FaceHandle>*
GpuSubdivision::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool
GpuSubdivision::FaceIterator::hasNext() const
{
    
    size_t n = _surface->numFaces();
    
    return n > 0 && _index != _surface->_d->_faces->end();
}

bool
GpuSubdivision::FaceIterator::hasPrevious() const
{
    size_t n = _surface->numFaces();
    
    return n > 0 &&
    (_index == _surface->_d->_faces->end() ||
     _index != _surface->_d->_faces->begin());
}

GpuSubdivision::FaceIterator::shared_ptr
GpuSubdivision::FaceIterator::next()
{
    auto *v = &*_index;
    ++_index;
    return v;
}

GpuSubdivision::FaceIterator::const_shared_ptr
GpuSubdivision::FaceIterator::next() const
{
    auto *v = &*_index;
    ++_index;
    return v;
}

GpuSubdivision::FaceIterator::shared_ptr
GpuSubdivision::FaceIterator::peekNext()
{
    return &*_index;
}

GpuSubdivision::FaceIterator::const_shared_ptr
GpuSubdivision::FaceIterator::peekNext() const
{
    return &*_index;
}


GpuSubdivision::FaceIterator::shared_ptr
GpuSubdivision::FaceIterator::previous()
{
    NOT_IMPLEMENTED
    return nullptr;
}

GpuSubdivision::FaceIterator::const_shared_ptr
GpuSubdivision::FaceIterator::previous() const
{
    NOT_IMPLEMENTED
    return nullptr;
}

bool
GpuSubdivision::FaceIterator::seek(int pos, IteratorOrigin origin) const
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
    
    _index += pos;
    if (_index == _surface->_d->_faces->end())
        return false;
    
    return true;
}

void
GpuSubdivision::printMemoryInfo() const
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
        std::string kernelSource = core::cl::loadFromFile(path);
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
        TRACE(error) << "ERROR: " << err.what() << "(" << err.err() << ")";
    }
    
    return err;
}

void
GpuSubdivision::Impl::subdivide(GpuSubdivision * s)
{
    if (!oclInitialized) {
        initialize_ocl();
        oclInitialized = true;
    }
}

void
GpuSubdivision::Impl::render(RenderState & state) const
{
    //assert(state.isValid() && "state is not valid");
    assert(_renderable != nullptr);
    _renderable->render(state);
}

void 
GpuSubdivision::Impl::copyToDevice()
{

}

}; /* namespace core */



namespace
{

IIterator<VertexHandle>*
FaceVertexIterator::clone() const
{
    auto cpy = new FaceVertexIterator(surface, face);
    cpy->currentEdge = currentEdge;
    return cpy;
}

bool
FaceVertexIterator::hasNext() const
{
    assert(surface && face && "surface or face are null");
    TRACE(debug) << "hasNext: Face->edge=" << face->edge << " currentEdge->next =" << currentEdge->next;
    return currentEdge->iid() != Edge::EDGE_END.iid();
}

bool
FaceVertexIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
}

VertexHandle::shared_ptr
FaceVertexIterator::next()
{
    assert(surface && face && "surface or face are null");
    assert(currentEdge->iid() && "currentEdge is the end");
    currentEdge = &surface->edge(currentEdge->next)->cast<const core::gpusubdivision::Edge>();
    TRACE(debug) << "    next:" << currentEdge->iid();
    return const_cast<const VertexHandle::shared_ptr>(surface->vertex(currentEdge->head));;
}

VertexHandle::const_shared_ptr
FaceVertexIterator::next() const
{
    assert(surface && face && "surface or face are null");
    currentEdge = &surface->edge(currentEdge->next)->cast<const core::gpusubdivision::Edge>();
    TRACE(debug) << "    next:" << currentEdge->iid();
    return const_cast<const VertexHandle::shared_ptr>(surface->vertex(currentEdge->head));;
}

VertexHandle::shared_ptr
FaceVertexIterator::peekNext()
{
    assert(surface && face && "surface or face are null");
    TRACE(debug) << "    current:" << currentEdge->iid();
    return const_cast<VertexHandle::shared_ptr>(surface->vertex(currentEdge->head));
}

VertexHandle::const_shared_ptr
FaceVertexIterator::peekNext() const 
{
    assert(surface && face && "surface or face are null");
    TRACE(debug) << "    current:" << currentEdge->iid();
    return const_cast<VertexHandle::shared_ptr>(surface->vertex(currentEdge->head));
}

VertexHandle::shared_ptr
FaceVertexIterator::previous()
{
    NOT_IMPLEMENTED
}

VertexHandle::const_shared_ptr
FaceVertexIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool
FaceVertexIterator::seek(int pos, IteratorOrigin origin) const 
{
    switch (origin)
    {
    case Iter_Current:
        //nothing
        break;
    case Iter_End:
        currentEdge = & Edge::EDGE_END;
        TRACE(debug) << "Iter_End: current:" << currentEdge->iid();
        break;

    case Iter_Start:
    default:
        currentEdge = &(surface->edge(face->edge)->cast<const Edge>());
        break;
    }

    if (pos > 0)
    {
        int step = 0;
        while (-1 != currentEdge->next && step < pos)
        {
            currentEdge = &(surface->edge(currentEdge->next)->cast<const Edge>());
            ++pos;
        }
    }
    else if (pos == 0)
    {
        // Nothing to do
    }
    else
    {
        return false;
    }
    
    return true;
}

}; // namespace anonymous


