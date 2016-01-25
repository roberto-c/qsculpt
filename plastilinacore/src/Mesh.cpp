/***************************************************************************
 *   Copyright (C) 2011 by Juan Roberto Cabral Flores   *
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
#include <PlastilinaCore/Mesh.h>

#include <math.h>
#include <Eigen/Core>
#include <map>
#include <unordered_map>
#include <atomic>

#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/RenderState.h>

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

using Vertex = core::subdivision::Vertex;
using Edge = core::subdivision::Edge;
using Face = core::subdivision::Face;

typedef std::pair<VertexHandle::size_t, VertexHandle::size_t> VtxPair;
typedef std::unordered_map<VertexHandle::size_t, Vertex*>   VertexCollection;
typedef std::unordered_map<EdgeHandle::size_t, Edge*>       EdgesCollection;
typedef std::unordered_map<VtxPair, EdgeHandle::size_t>       VtxPairEdgeMap;
typedef std::map<FaceHandle::size_t, Face*>                 FacesCollection;


struct VertexPool {
    std::vector<size_t> free;
    std::vector<Vertex> element;
};

static std::atomic_int NEXT_ID;

struct Mesh::Impl {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /** Instance ID of the surface */
	ISurface::size_t _iid;
    
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
    bool            _hasChanged;
    std::vector<size_t>    _selectedPoints;
    
    VertexPool      _vtxPool;
    
    Impl() :     _vertices(NULL),
    _edges(NULL),
    _vtxPairEdge(NULL),
    _faces(NULL),
    _scene(NULL),
    //_drawingMode(Wireframe),
    _color(1.f,1.f,1.f),
    _selected(false),
    _callListId(0),
    _genereateCallList(true),
    _hasChanged(true)
    {
        
    }
};

class MeshRenderer : public IRenderer
{
	static MeshRenderer * instamce_;
	
public:
	static MeshRenderer * instance() {
		if (MeshRenderer::instamce_ == NULL) {
			MeshRenderer::instamce_ = new MeshRenderer;
		}
		return MeshRenderer::instamce_;
	}
	
	void renderObject(std::shared_ptr<SceneNode> & node) {
		NOT_IMPLEMENTED;
	}
	
	void render(const Mesh* mesh, RenderState & state) const {
		NOT_IMPLEMENTED;
	}
};

MeshRenderer * MeshRenderer::instamce_ = NULL;


// Iterator classes declarations
class Mesh::VertexIterator : public IIterator<VertexHandle>
{	
public:
    typedef Vertex::shared_ptr shared_ptr;
    typedef Vertex::weak_ptr weak_ptr;
    typedef Vertex::ptr ptr;
    
private:
    friend class Mesh;
public:
    typedef VertexCollection::iterator iterator;
    
    const Mesh*         _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(const Mesh* surface, int level = -1);
    
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
};

class Mesh::FaceIterator : public IIterator<FaceHandle>
{
public:
    typedef FaceHandle::shared_ptr   shared_ptr;
    typedef FaceHandle::weak_ptr     weak_ptr;
    typedef FaceHandle::ptr          ptr;
    
private:
    friend class Mesh;
    
    typedef FacesCollection::iterator iterator;
    
    char tmp[4];
    const Mesh*         _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(const Mesh* v, int level = -1);
    
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
};

class Mesh::EdgeIterator : public IIterator<EdgeHandle>
{
public:
    typedef Edge::shared_ptr shared_ptr;
    typedef Edge::weak_ptr weak_ptr;
    typedef Edge::ptr ptr;
    
private:
    friend class Mesh;
    
    typedef EdgesCollection::iterator iterator;
    
    const Mesh*         surface_;
    int                 level_;
    mutable iterator    index_;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator(const Mesh* s, int level = -1) 
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
    const_shared_ptr next() const {
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
    const_shared_ptr peekNext() const {
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

Mesh::Mesh()
:   ISurface(),
_d(new Impl)
{
    _d->_iid = NEXT_ID.fetch_add(1);
    initPoints();
    //updateBoundingBox();
}

Mesh::~Mesh()
{
    delete _d->_vertices;
    delete _d->_edges;
    delete _d->_vtxPairEdge;
    delete _d->_faces;
}

ISurface::size_t Mesh::iid() const
{
    return _d->_iid;
}

void Mesh::initPoints()
{
    _d->_vertices = new VertexCollection;
    _d->_edges = new EdgesCollection;
    _d->_vtxPairEdge = new VtxPairEdgeMap;
    _d->_faces = new FacesCollection;
    
    _d->_vertices->reserve(50000);
    _d->_edges->reserve(50000);
    _d->_vtxPairEdge->reserve(50000);
}

bool Mesh::hasChanged() {
    return _d->_hasChanged;
}

void Mesh::setChanged(bool val) {
    _d->_hasChanged = val;
    if (val) {
        meshChanged(this);
    }
}

std::vector<VertexHandle::size_t> Mesh::selectedPoints() const {
    return _d->_selectedPoints;
}

void Mesh::setSelectedPoints(const std::vector<VertexHandle::size_t>& indicesArray) {
    _d->_selectedPoints = indicesArray;
}

void Mesh::setScene(Scene* scene)
{
    _d->_scene = scene;
}

Scene* Mesh::scene() const
{
    return _d->_scene;
}

const IRenderable* Mesh::renderable() const
{
	return this;
}

// IRenderable
void Mesh::render(RenderState & state) const
{
	MeshRenderer::instance()->render(this, state);
}
// End IRendeable

const geometry::AABB& Mesh::boundingBox() const
{
    return _d->_boundingBox;
}

void Mesh::setColor(const Color& color)
{
    _d->_color = color;
}

Color Mesh::color() const
{
    return _d->_color;
}

void Mesh::setSelected(bool val)
{
    _d->_selected = val;
}

bool Mesh::isSelected() const
{
    return _d->_selected;
}

VertexHandle::size_t Mesh::addVertex(const Point3& point)
{
    assert(_d->_vertices != NULL );
    //qWarning("%s %s", __FUNCTION__, " Not implemented");
    
    ::Vertex* vertex = new ::Vertex(point);
    _d->_boundingBox.extend(point);
    VertexCollection::value_type value(vertex->iid(), vertex);
    this->_d->_vertices->insert(value);
    return vertex->iid();
    
}

VertexHandle::size_t Mesh::addVertex(VertexHandle* vp)
{
    ::Vertex *v = static_cast<::Vertex*>(vp);
    _d->_boundingBox.extend(v->position());
    VertexCollection::value_type value(v->iid(), v);
    this->_d->_vertices->insert(value);
    return v->iid();
}

void Mesh::removeVertex(VertexHandle::size_t iid)
{
    _d->_vertices->erase(iid);
}

VertexHandle* Mesh::vertex(VertexHandle::size_t iid)
{
    ::Vertex *v = _d->_vertices->find(iid) != _d->_vertices->end() ?
            _d->_vertices->at(iid) : NULL;
    return v;
}

const VertexHandle* Mesh::vertex(VertexHandle::size_t iid) const
{
    return _d->_vertices->find(iid) != _d->_vertices->end() ?
        _d->_vertices->at(iid) : NULL;
}

VertexHandle::size_t Mesh::numVertices() const
{
    return static_cast<VertexHandle::size_t>(_d->_vertices->size());
}

EdgeHandle::size_t Mesh::addEdge(const EdgeHandle& e)
{
    const Edge * edge = static_cast<const Edge *>(&e);
    return addEdge(edge->pair()->head(), edge->head());
}

EdgeHandle::size_t Mesh::addEdge(VertexHandle::size_t v1, VertexHandle::size_t v2)
{    
    return addEdge(_d->_vertices->at(v1), _d->_vertices->at(v2));
}

EdgeHandle::size_t Mesh::addEdge(VertexHandle* t, VertexHandle* h)
{
    assert(t && h);
    Vertex * tail = static_cast<Vertex*>(t);
    Vertex * head = static_cast<Vertex*>(h);
	Edge::size_t iid = -1;
    
    VtxPair pair(tail->iid(), head->iid());
    if (_d->_vtxPairEdge->find(pair) != _d->_vtxPairEdge->end()) {
        iid = _d->_vtxPairEdge->at(pair);
    } else {
        Edge *h1 = new Edge(tail, head);
        Edge *h2 = new Edge(head, tail);
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

EdgeHandle::size_t Mesh::numEdges() const {
	return static_cast<EdgeHandle::size_t>(_d->_edges->size());
}

FaceHandle::size_t Mesh::addFace(const std::vector<VertexHandle::size_t>& vertexIndexList)
{
    //NOT_IMPLEMENTED
    std::vector<Edge*> edges;
    std::vector<size_t>::size_type size = vertexIndexList.size();
    if (size < 3) {
        std::cerr << "addFace: not enough vertices: " << size << std::endl;
        return -1;
    }

    std::vector<size_t>::size_type i = 0;
    for (i = 0; i < size; ++i) {
        size_t iid = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
        assert(iid > 0);
        Edge *e = _d->_edges->at(iid);
        assert(e != NULL && e->face() == NULL);
        edges.push_back(e);
    }
    Face *f = new Face(this);
    f->setHEdge(edges[0]);
    for (size_t i = 0; i < size; ++i) {
        edges[i]->setNext(edges[(i+1)%size]);
        edges[i]->setFace(f);
        //_edges[edgesIndices[i]].setNext(edgesIndices[(i+1)%size]);
    }
    _d->_faces->insert(std::pair<int,Face*>(f->iid(), f));
    return f->iid();
}

void Mesh::replaceFace(FaceHandle::size_t /*faceIndex*/, const std::vector<VertexHandle::size_t>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void Mesh::removeFace( FaceHandle::size_t iid)
{
    Face * f = static_cast<Face*>(face(iid));
    if (f == NULL)
        return;
    Iterator<EdgeHandle> edgeIt = f->edgeIterator();
    while (edgeIt.hasNext()) {
        auto e = static_cast<Edge*>(edgeIt.next());
        e->setFace(NULL);
        e->setNext(NULL);
    }
    _d->_faces->erase(iid);
}

FaceHandle* Mesh::face(FaceHandle::size_t iid)
{
    assert(iid > 0);
    FacesCollection::iterator it = _d->_faces->find(iid);
    return  it != _d->_faces->end() ? it->second : NULL;
}

FaceHandle::size_t Mesh::numFaces() const
{
    assert( _d->_faces!= NULL );
    return static_cast<Face::size_t>(_d->_faces->size());
}


FaceHandle::size_t Mesh::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

VertexHandle::size_t Mesh::getClosestPointAtPoint(const Point3 &/*p*/) const
{
    NOT_IMPLEMENTED
    
    return -1;
}

std::vector<VertexHandle::size_t> Mesh::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //std::cerr << "Mesh::getPointsInRadius()";
    std::vector<size_t> results;
    
    NOT_IMPLEMENTED
    
    return results;
}

void Mesh::lock() const
{
    NOT_IMPLEMENTED
    //_mutex.lock();
}

void Mesh::unlock() const
{
    NOT_IMPLEMENTED
    //_mutex.unlock();
}

void Mesh::adjustPointNormal(size_t /*index*/)
{
    NOT_IMPLEMENTED
}

Iterator<VertexHandle> Mesh::vertexIterator()
{
    return Iterator<VertexHandle>(new VertexIterator(this) );
}

Iterator<VertexHandle> Mesh::constVertexIterator() const
{
    return Iterator<VertexHandle>(new VertexIterator(this) );
}

Iterator<FaceHandle> Mesh::faceIterator()
{
    return Iterator<FaceHandle>(new FaceIterator(this));
}

Iterator<FaceHandle> Mesh::constFaceIterator() const
{
    return Iterator<FaceHandle>(new FaceIterator(this));
}

Iterator<EdgeHandle> Mesh::edgeIterator()
{
    return Iterator<EdgeHandle>(new EdgeIterator(this));
}

Iterator<EdgeHandle> Mesh::constEdgeIterator() const
{
    return Iterator<EdgeHandle>(new EdgeIterator(this));
}

// Inner classes implementation
// Mesh::VertexIterator
Mesh::VertexIterator::VertexIterator(const Mesh* surface, int level)
:	_surface(surface),
_level(level)
{
    _index = _surface->_d->_vertices->begin();
}


IIterator<VertexHandle>* Mesh::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Mesh::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n >0 && _index != _surface->_d->_vertices->end();
}

bool Mesh::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n > 0 &&
    (_index == _surface->_d->_vertices->end() ||
     _index != _surface->_d->_vertices->begin());
}

Mesh::VertexIterator::shared_ptr Mesh::VertexIterator::next()
{
    Vertex *v = _index->second;
    ++_index;
    return v;
}

Mesh::VertexIterator::const_shared_ptr Mesh::VertexIterator::next() const
{
    Vertex *v = _index->second;
    ++_index;
    return v;
}

Mesh::VertexIterator::shared_ptr Mesh::VertexIterator::peekNext() {
    return _index->second;
}

Mesh::VertexIterator::const_shared_ptr Mesh::VertexIterator::peekNext() const {
    return _index->second;
}

Mesh::VertexIterator::shared_ptr Mesh::VertexIterator::previous()
{
    NOT_IMPLEMENTED
//    --_index;
//    return _index->second;
}

Mesh::VertexIterator::const_shared_ptr Mesh::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
//    --_index;
//    return _index->second;
}

bool Mesh::VertexIterator::seek(int pos, IteratorOrigin origin) const
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
        std::cerr << "Mesh::VertexIterator::seek: not supported" << std::endl;
        return false;
//        while(++pos && _index != _surface->_d->_vertices->end()) {
//            --_index;
//        }
//        if (_index == _surface->_d->_vertices->end())
//            return false;
    }
    return true;
}

//Mesh::FaceIterator
Mesh::FaceIterator::FaceIterator(const Mesh* surface, int level)
:	_surface(surface),
_level(level)
{
    assert(surface);
    this->tmp[0] = 'a';
    this->tmp[1] = 'b';
    _index = _surface->_d->_faces->begin();
}

IIterator<FaceHandle>* Mesh::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Mesh::FaceIterator::hasNext() const
{
    size_t n = _surface->numFaces();
    return n > 0 && _index != _surface->_d->_faces->end();
}

bool Mesh::FaceIterator::hasPrevious() const
{
    size_t n = _surface->numFaces();

    return n > 0 &&
            (_index == _surface->_d->_faces->end() ||
            _index != _surface->_d->_faces->begin());
}

Mesh::FaceIterator::shared_ptr Mesh::FaceIterator::next()
{
    Face *f = _index->second;
    
    assert(f);
    ++_index;
    return f;
}

Mesh::FaceIterator::const_shared_ptr Mesh::FaceIterator::next() const
{
    Face *f = _index->second;
    assert(f);
    ++_index;
    return f;
}

Mesh::FaceIterator::shared_ptr Mesh::FaceIterator::peekNext() {
    return _index->second;
}

Mesh::FaceIterator::const_shared_ptr Mesh::FaceIterator::peekNext() const {
    return _index->second;
}


Mesh::FaceIterator::shared_ptr Mesh::FaceIterator::previous()
{
    --_index;
    return _index->second;
}

Mesh::FaceIterator::const_shared_ptr Mesh::FaceIterator::previous() const
{
    --_index;
    return _index->second;
}

bool Mesh::FaceIterator::seek(int pos, IteratorOrigin origin) const
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

void Mesh::printMemoryInfo() const
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



