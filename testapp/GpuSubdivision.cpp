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

typedef std::pair<Vertex::size_t, Vertex::size_t>     VtxPair;
typedef std::unordered_map<Vertex::size_t, Vertex*>   VertexCollection;
typedef std::unordered_map<Edge::size_t, Edge*>       EdgesCollection;
typedef std::unordered_map<VtxPair, Edge::size_t>     VtxPairEdgeMap;
typedef std::map<Face::size_t, Face*>                 FacesCollection;


struct VertexPool {
    std::vector<size_t> free;
    std::vector<Vertex> element;
    
    VertexPool();
    
    Vertex * allocate(const Point3 & p);
    void delloacate(Vertex * ptr);
};

VertexPool::VertexPool() {
    element.reserve(1000);
}

Vertex * VertexPool::allocate(const Point3 &p) {
    element.insert(element.end(), p);
    return &element.at(element.size() - 1);
    //return new Vertex(p);
}

void VertexPool::delloacate(Vertex *ptr) {
    //delete ptr;
}

struct EdgePool {
    std::vector<size_t> free;
    std::vector<Edge> element;
    
    EdgePool();
    
    Edge * allocate(Vertex * v1, Vertex * v2);
    void delloacate(Edge * ptr);
};

EdgePool::EdgePool() {
    element.reserve(1000);
}

Edge * EdgePool::allocate(Vertex * v1, Vertex * v2) {
    //element.insert(element.end(), Edge(v1, v2));
    //return &element.at(element.size() - 1);
    return new Edge(v1, v2);
}

void EdgePool::delloacate(Edge *ptr) {
    //delete ptr;
}

static std::atomic_int NEXT_ID;

namespace core {
    struct clSurfaceList;
    
    struct GpuSubdivision::Impl : public IRenderable {
    /** Instance ID of the surface */
    ISurface::size_t _iid;
    
    /** Vertices by each subdivision level */
    std::vector<VertexCollection*>  _vertLevelCollections;
    /** Edges by each subdivision level */
    std::vector<EdgesCollection*>   _edgesLevelCollections;
    /** Relates a pair of vertex ids to an edge*/
    std::vector<VtxPairEdgeMap*>    _vertexEdgeCollection;
    
    std::vector<FacesCollection*>   _facesLevelCollections;
    
    
    VertexCollection    *_vertices;
    EdgesCollection     *_edges;
    VtxPairEdgeMap      *_vtxPairEdge;
    FacesCollection     *_faces;
    
    SubdivisionScheme* scheme;
    
    Scene*          _scene;
    Color           _color;
    geometry::AABB  _boundingBox;
    bool            _selected;
    int             _callListId;
    bool            _genereateCallList;
    int             _currentResolutionLevel;
    bool            _hasChanged;
    std::vector<Vertex::size_t>    _selectedPoints;
    
    VertexPool      _vtxPool;
    EdgePool        _edgePool;
    
    static bool 				oclInitialized;
    static cl::Program  		program;
    static cl::Kernel			krnInit;
    static cl::Kernel 			krnSubdivideEdges;
    static cl::Kernel			krnSubdivideAddFaces;
    static cl::Kernel			krnSubdivideAdjustPos;
    
    Impl(GpuSubdivision * surface) :     _vertices(NULL),
    _edges(NULL),
    _vtxPairEdge(NULL),
    _faces(NULL),
    _scene(NULL),
    //_drawingMode(Wireframe),
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
cl::Program  		GpuSubdivision::Impl::program;
cl::Kernel			GpuSubdivision::Impl::krnInit;
cl::Kernel 			GpuSubdivision::Impl::krnSubdivideEdges;
cl::Kernel			GpuSubdivision::Impl::krnSubdivideAddFaces;
cl::Kernel			GpuSubdivision::Impl::krnSubdivideAdjustPos;

// Iterator classes declarations
class GpuSubdivision::VertexIterator : public IIterator<Vertex>
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
    IIterator<Vertex>* clone() const;
    
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

class GpuSubdivision::FaceIterator : public IIterator<Face>
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
    IIterator<Face>* clone() const;
    
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

class GpuSubdivision::EdgeIterator : public IIterator<Edge>
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
    IIterator<Edge>* clone() const {
        EdgeIterator *it = new EdgeIterator(surface_, level_);
        it->index_ = index_;
        return it;
    }
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const {
        return (index_ != surface_->_d->_edges->end()) && (index_->second != NULL);
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
        Edge * e = index_->second;
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const {
        Edge * e = index_->second;
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext() {
        return index_->second;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const {
        return index_->second;
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
    _d->_vtxPairEdge = NULL;
    _d->_faces = NULL;
    
    std::vector<VertexCollection*>::iterator it = _d->_vertLevelCollections.begin();
    for (; it != _d->_vertLevelCollections.end(); ++it)
    {
        _d->_vertices = (*it);
        //Vertex
    }
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
    _d->_vtxPairEdge = new VtxPairEdgeMap;
    _d->_faces = new FacesCollection;
    
    _d->_vertices->reserve(50000);
    _d->_edges->reserve(50000);
    _d->_vtxPairEdge->reserve(50000);
    
    _d->_vertLevelCollections.push_back(_d->_vertices);
    _d->_edgesLevelCollections.push_back(_d->_edges);
    _d->_vertexEdgeCollection.push_back(_d->_vtxPairEdge);
    _d->_facesLevelCollections.push_back(_d->_faces);
    
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

std::vector<Vertex::size_t> GpuSubdivision::selectedPoints() const {
    return _d->_selectedPoints;
}

void GpuSubdivision::setSelectedPoints(const std::vector<Vertex::size_t>& indicesArray) {
    _d->_selectedPoints = indicesArray;
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
        it.next()->color() = color;
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

Vertex::size_t GpuSubdivision::addVertex(const Point3& point)
{
    assert(_d->_vertices != NULL );
    //qWarning("%s %s", __FUNCTION__, " Not implemented");
    
    //    Vertex* vertex = _d->_vtxPool.allocate(point);// new Vertex(point);
    Vertex* vertex = new Vertex(point);
    vertex->color() = _d->_color;
    //    size_t index = _d->_vtxPool.free[0];
    //    _d->_vtxPool.free.pop_back();
    //    Vertex* vertex = NULL;
    //    vertex = &_d->_vtxPool.element.at(_d->_vtxPool.element.size());
    //    vertex->position() = point;
    _d->_boundingBox.extend(point);
    VertexCollection::value_type value(vertex->iid(), vertex);
    this->_d->_vertices->insert(value);
    return vertex->iid();
    
}

Vertex::size_t GpuSubdivision::addVertex(Vertex* v)
{
    _d->_boundingBox.extend(v->position());
    VertexCollection::value_type value(v->iid(), v);
    this->_d->_vertices->insert(value);
    return v->iid();
}

void GpuSubdivision::removeVertex(Vertex::size_t iid)
{
    _d->_vertices->erase(iid);
}

Vertex* GpuSubdivision::vertex(Vertex::size_t iid)
{
    return _d->_vertices->find(iid) != _d->_vertices->end() ?
    _d->_vertices->at(iid) : NULL;
}

const Vertex* GpuSubdivision::vertex(size_t iid) const
{
    return _d->_vertices->find(iid) != _d->_vertices->end() ?
    _d->_vertices->at(iid) : NULL;
}

Vertex::size_t GpuSubdivision::numVertices() const
{
    return static_cast<Vertex::size_t>(_d->_vertices->size());
}

Edge::size_t GpuSubdivision::addEdge(const Edge& edge)
{
    return addEdge(edge.pair()->head(), edge.head());
}

Edge::size_t GpuSubdivision::addEdge(Vertex::size_t v1, Vertex::size_t v2)
{
    return addEdge(_d->_vertices->at(v1), _d->_vertices->at(v2));
}

Edge::size_t GpuSubdivision::addEdge(Vertex* tail, Vertex* head)
{
    assert(tail && head);
    
    Edge::size_t iid = -1;
    
    VtxPair pair(tail->iid(), head->iid());
    if (_d->_vtxPairEdge->find(pair) != _d->_vtxPairEdge->end()) {
        iid = _d->_vtxPairEdge->at(pair);
    } else {
        Edge *h1 = _d->_edgePool.allocate(tail, head); // new Edge(tail, head);
        Edge *h2 = _d->_edgePool.allocate(head, tail); //new Edge(head, tail);
        h1->setPair(h2);
        h2->setPair(h1);
        
        h1->head()->setEdge(h2);
        
        iid = h1->iid();
        _d->_edges->insert(EdgesCollection::value_type(h1->iid(), h1));
        _d->_edges->insert(EdgesCollection::value_type(h2->iid(), h2));
        _d->_vtxPairEdge->insert(VtxPairEdgeMap::value_type(pair, h1->iid()));
        pair.first = head->iid();
        pair.second = tail->iid();
        _d->_vtxPairEdge->insert(VtxPairEdgeMap::value_type(pair, h2->iid()));
        
        if (tail->isFlagSet(VF_Crease) && head->isFlagSet(VF_Crease)) {
            h1->addFlag(EF_Crease);
            h2->addFlag(EF_Crease);
        }
    }
    
    return iid;
}

Edge::size_t GpuSubdivision::edge(Vertex::size_t iidVtxTail, Vertex::size_t iidVtxHead) const
{
    assert(iidVtxTail > 0 && iidVtxHead > 0);
    
    size_t iid = 0;
    
    VtxPair pair(iidVtxTail, iidVtxHead);
    if (_d->_vtxPairEdge->find(pair) != _d->_vtxPairEdge->end()) {
        iid = _d->_vtxPairEdge->at(pair);
    }
    
    return iid;
}

Edge * GpuSubdivision::edge(Edge::size_t iidEdge)
{
    return _d->_edges->at(iidEdge);
}

const Edge * GpuSubdivision::edge(Edge::size_t iidEdge) const
{
    return _d->_edges->at(iidEdge);
}

Edge::size_t GpuSubdivision::numEdges() const
{
    return static_cast<Edge::size_t>(_d->_edges->size());
}

Face::size_t GpuSubdivision::addFace(const std::vector<Vertex::size_t>& vertexIndexList)
{
    //NOT_IMPLEMENTED
    std::vector<Edge*> edges;
    auto size = vertexIndexList.size();
    if (size < 3) {
        std::cerr << "addFace: not enough vertices: " << size << std::endl;
        return -1;
    }
    
    for (int i = 0; i < size; ++i) {
        size_t iid = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
        assert(iid > 0);
        Edge *e = _d->_edges->at(iid);
        assert(e != NULL && e->face() == NULL);
        edges.push_back(e);
    }
    Face *f = new Face(this);
    f->setHEdge(edges[0]);
    for (int i = 0; i < size; ++i) {
        edges[i]->setNext(edges[(i+1)%size]);
        edges[i]->setFace(f);
        //_edges[edgesIndices[i]].setNext(edgesIndices[(i+1)%size]);
    }
    _d->_faces->insert(std::pair<int,Face*>(f->iid(), f));
    return f->iid();
}

void GpuSubdivision::replaceFace(Face::size_t /*faceIndex*/, const std::vector<Vertex::size_t>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void GpuSubdivision::removeFace( Face::size_t iid)
{
    Face * f = face(iid);
    if (f == NULL)
        return;
    Iterator<Edge> edgeIt = f->edgeIterator();
    while (edgeIt.hasNext()) {
        auto e = edgeIt.next();
        e->setFace(NULL);
        e->setNext(NULL);
    }
    _d->_faces->erase(iid);
}

Face* GpuSubdivision::face(size_t iid)
{
    assert(iid > 0);
    FacesCollection::iterator it = _d->_faces->find(iid);
    return  it != _d->_faces->end() ? it->second : NULL;
}

Face::size_t GpuSubdivision::numFaces() const
{
    assert( _d->_faces!= NULL );
    return static_cast<Face::size_t>(_d->_faces->size());
}


Face::size_t GpuSubdivision::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

Vertex::size_t GpuSubdivision::getClosestPointAtPoint(const Point3 & p) const
{
    float d = MAXFLOAT;
    Vertex::shared_ptr vtx = nullptr, tmpVtx = nullptr;
    Point3 v;
    Iterator<Vertex> it = constVertexIterator();
    while (it.hasNext()) {
        tmpVtx = it.next();
        v = tmpVtx->position() - p;
        if (v.squaredNorm() < d) {
            d = v.squaredNorm();
            vtx = tmpVtx;
        }
    }
    
    return vtx ? vtx->iid() : 0;
}

std::vector<Vertex::size_t> GpuSubdivision::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //std::cerr << "GpuSubdivision::getPointsInRadius()";
    std::vector<Vertex::size_t> results;
    
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

Iterator<Vertex> GpuSubdivision::vertexIterator()
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Vertex> GpuSubdivision::constVertexIterator() const
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Face> GpuSubdivision::faceIterator()
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Face> GpuSubdivision::constFaceIterator() const
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Vertex> GpuSubdivision::vertexIterator(int level)
{
    return Iterator<Vertex>(new VertexIterator(this, level) );
}

Iterator<Vertex> GpuSubdivision::constVertexIterator(int level) const
{
    return Iterator<Vertex>(new VertexIterator(this, level) );
}

Iterator<Face> GpuSubdivision::faceIterator(int level)
{
    return Iterator<Face>(new FaceIterator(this, level));
}

Iterator<Face> GpuSubdivision::constFaceIterator(int level) const
{
    return Iterator<Face>(new FaceIterator(this, level));
}

Iterator<Edge> GpuSubdivision::edgeIterator()
{
    return Iterator<Edge>(new EdgeIterator(this));
}

Iterator<Edge> GpuSubdivision::constEdgeIterator() const
{
    return Iterator<Edge>(new EdgeIterator(this));
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


IIterator<Vertex>* GpuSubdivision::VertexIterator::clone() const
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
    Vertex *v = _index->second;
    ++_index;
    return v;
}

const GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::next() const
{
    Vertex *v = _index->second;
    ++_index;
    return v;
}

GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::peekNext() {
    return _index->second;
}

const GpuSubdivision::VertexIterator::shared_ptr GpuSubdivision::VertexIterator::peekNext() const {
    return _index->second;
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

IIterator<Face>* GpuSubdivision::FaceIterator::clone() const
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
    Face *f = _index->second;
    
    assert(f);
    ++_index;
    return f;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::next() const
{
    Face *f = _index->second;
    assert(f);
    ++_index;
    return f;
}

GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::peekNext() {
    return _index->second;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::peekNext() const {
    return _index->second;
}


GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::previous()
{
    --_index;
    return _index->second;
}

const GpuSubdivision::FaceIterator::shared_ptr GpuSubdivision::FaceIterator::previous() const
{
    --_index;
    return _index->second;
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
        cl::Program::Sources source(1,
                                    std::make_pair(kernelSource.c_str(),kernelSource.length()));
        program = cl::Program(oclManager->context(), source);
        program.build(oclManager->devices());
        
        krnInit = cl::Kernel(program,"surface_init",&err);
        krnSubdivideEdges = cl::Kernel(program, "subdivide_edges_midpoint", &err);
        std::cout << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: " << krnSubdivideEdges.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(NULL) << std::endl;
        krnSubdivideAddFaces = cl::Kernel(program, "subdivide_add_faces", &err);
        krnSubdivideAdjustPos = cl::Kernel(program,"subdivide_vertices_adjust_pos", &err);
        std::cout << "CL_KERNEL_COMPILE_WORK_GROUP_SIZE: " << krnSubdivideAdjustPos.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(NULL) << std::endl;
    }
    catch (cl::Error err) {
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
    cl_float4 	n;	// notmal
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
    uint 					num_polygons;
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
