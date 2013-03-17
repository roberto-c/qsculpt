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
#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>
#include "cl.hpp"
#include <math.h>
#include <Eigen/Core>
#include <map>
#include <unordered_map>
#include <atomic>

#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Mesh.h>
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>
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

struct clSurfaceList;

struct Subdivision::Impl {
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
	
	SubdivisionRenderable renderable;
	
	std::unordered_map<Vertex::size_t, Vertex::size_t> hndVtxIndex;
	std::unordered_map<Edge::size_t, Edge::size_t> hndEdgeIndex;
	std::unordered_map<Face::size_t, Face::size_t> hndFaceIndex;
	bool 				oclInitialized;
	cl::Program 		program;
	cl::Kernel			krnInit;
	cl::Kernel 			krnSubdivideEdges;
	cl::Kernel 			krnSubdivideFaces;
	cl::Kernel			krnSubdivideAddFaces;
    
    Impl(Subdivision * surface) :     _vertices(NULL),
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
    _hasChanged(true),
	renderable(surface),
	oclInitialized(false)
    {
    }
    
    void 		subdivide(Subdivision * surf);
	void 		subdivide_ocl(Subdivision * surf);
	
	int 		initialize_ocl(void);
	
	struct clSurfaceList	create_from_surface(Subdivision * surface);
	void 					update_surface_from_polygon(Subdivision * surface,
														clSurfaceList & list);
};

// Iterator classes declarations
class Subdivision::VertexIterator : public IIterator<Vertex>
{	
    friend class Subdivision;
public:
    typedef VertexCollection::iterator iterator;

    const Subdivision*  _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(const Subdivision* surface, int level = -1);
    
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

class Subdivision::FaceIterator : public IIterator<Face>
{
    friend class Subdivision;

    typedef FacesCollection::iterator iterator;

    char tmp[4];
    const Subdivision*  _surface;
    int                 _level;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(const Subdivision* v, int level = -1);
    
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

class Subdivision::EdgeIterator : public IIterator<Edge>
{
    friend class Subdivision;
    
    typedef EdgesCollection::iterator iterator;
    
    const Subdivision*  surface_;
    int                 level_;
    mutable iterator    index_;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator(const Subdivision* s, int level = -1) 
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

void Subdivision::Impl::subdivide(Subdivision * surf) {
    assert(surf);
    std::vector<Face::size_t> faceToDelete;
    VtxPairEdgeMap edgePoints;
    Point3 p;
    
    auto edgeIt = surf->edgeIterator();
    while(edgeIt.hasNext()) {
        auto edge = edgeIt.next();
        if (edge->userData()) continue;
        p = (edge->head()->position() + edge->tail()->position() ) / 2.0f;
        size_t midPointIid = surf->addVertex(p);
        edge->setUserData( (void*)(midPointIid) );
        edge = edge->pair();
        if (edge) {
            edge->setUserData( (void*)(midPointIid) );
        }
    }
    
    auto faceIt = surf->faceIterator();
    while (faceIt.hasNext()) {
        auto face = faceIt.next();
        if (face->numVertices() < 3 )
            continue;
        
        if (face->userData() || face->isFlagSet(FF_User1)) continue;
        faceToDelete.push_back(face->iid());
        p = Point3();
        auto vtxIt = face->vertexIterator();
        while(vtxIt.hasNext()) {
            p += vtxIt.next()->position();
        }
        p = p / face->numVertices();
        size_t midPointIid = surf->addVertex(p);
        face->setUserData( static_cast<void*>(&midPointIid) );
        
        auto edgeIt = face->edgeIterator();
        auto edge1 = edgeIt.next();
        while (edgeIt.hasNext()) {
            auto edge2 = edgeIt.next();
			Edge::size_t edgeMidPoint1 = static_cast<Edge::size_t>(::size_t(edge1->userData()));
            Edge::size_t edgeMidPoint2 = static_cast<Edge::size_t>(::size_t(edge2->userData()));
            assert(edgeMidPoint1 && edgeMidPoint2);
            std::vector<size_t> index;
            index.push_back(edgeMidPoint1);
            index.push_back(edge1->head()->iid());
            index.push_back(edgeMidPoint2);
            index.push_back(midPointIid);
            size_t faceiid = surf->addFace(index);
            surf->face(faceiid)->setFlags(FF_User1);
            
            edge1 = edge2;
        }
        edgeIt.seek(0);
        auto edge2 = edgeIt.next();
		Edge::size_t edgeMidPoint1 = static_cast<Edge::size_t>(::size_t(edge1->userData()));
		Edge::size_t edgeMidPoint2 = static_cast<Edge::size_t>(::size_t(edge2->userData()));
        assert(edgeMidPoint1 && edgeMidPoint2);
        std::vector<size_t> index;
        index.push_back(edgeMidPoint1);
        index.push_back(edge1->head()->iid());
        index.push_back(edgeMidPoint2);
        index.push_back(midPointIid);
        size_t faceiid = surf->addFace(index);
        surf->face(faceiid)->setFlags(FF_User1);
    }
    
    for (Face::size_t i = 0; i < faceToDelete.size(); ++i) {
        surf->removeFace(faceToDelete[i]);
    }
    
    faceIt = surf->faceIterator();
    while (faceIt.hasNext()) {
        faceIt.next()->removeFlag(FF_User1);
    }
}

Subdivision::Subdivision()
    :   ISurface(),
    _d(new Impl(this))
{
    _d->_iid = NEXT_ID.fetch_add(1);
    initPoints();
    //updateBoundingBox();
}

Subdivision::Subdivision(const Mesh * mesh)
: ISurface(),
_d(new Impl(this))
{
    convertFromMesh(mesh);
}

Subdivision::~Subdivision()
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

ISurface::size_t Subdivision::iid() const
{
    return _d->_iid;
}

void Subdivision::initPoints()
{
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
    
//    _d->_vtxPool.free.reserve(1000000);
//    for (size_t i = 0; i < 1000000; ++i) {
//        _d->_vtxPool.free.push_back(i);
//    }
//    _d->_vtxPool.element.reserve(1000000);
//    _d->_vtxPool.element.resize(1000000);
}

bool Subdivision::hasChanged() {
    return _d->_hasChanged;
}

void Subdivision::setChanged(bool val) {
    _d->_hasChanged = val;
    if (val) {
        meshChanged(this);
    }
}

std::vector<Vertex::size_t> Subdivision::selectedPoints() const {
    return _d->_selectedPoints;
}

void Subdivision::setSelectedPoints(const std::vector<Vertex::size_t>& indicesArray) {
    _d->_selectedPoints = indicesArray;
}

void Subdivision::setScene(Scene* scene)
{
    _d->_scene = scene;
}

Scene* Subdivision::scene() const
{
    return _d->_scene;
}

const IRenderable* Subdivision::renderable() const
{
	return &_d->renderable;
}

const geometry::AABB& Subdivision::boundingBox() const
{
    return _d->_boundingBox;
}

void Subdivision::setColor(const Color& color)
{
    _d->_color = color;
    auto it = vertexIterator();
    while (it.hasNext()) {
        it.next()->color() = color;
    }
}

Color Subdivision::color() const
{
    return _d->_color;
}

void Subdivision::setSelected(bool val)
{
    _d->_selected = val;
}

bool Subdivision::isSelected() const
{
    return _d->_selected;
}

Vertex::size_t Subdivision::addVertex(const Point3& point)
{
    assert(_d->_vertices != NULL );
    //qWarning("%s %s", __FUNCTION__, " Not implemented");
    
    Vertex* vertex = _d->_vtxPool.allocate(point);// new Vertex(point);
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

Vertex::size_t Subdivision::addVertex(Vertex* v)
{
    _d->_boundingBox.extend(v->position());
    VertexCollection::value_type value(v->iid(), v);
    this->_d->_vertices->insert(value);
    return v->iid();
}

void Subdivision::removeVertex(Vertex::size_t iid)
{
    _d->_vertices->erase(iid);
}

Vertex* Subdivision::vertex(Vertex::size_t iid)
{
    return _d->_vertices->find(iid) != _d->_vertices->end() ?
        _d->_vertices->at(iid) : NULL;
}

const Vertex* Subdivision::vertex(size_t iid) const
{
    return _d->_vertices->find(iid) != _d->_vertices->end() ?
        _d->_vertices->at(iid) : NULL;
}

Vertex::size_t Subdivision::numVertices() const
{
    return static_cast<Vertex::size_t>(_d->_vertices->size());
}

Edge::size_t Subdivision::addEdge(const Edge& edge)
{
    return addEdge(edge.tail(), edge.head());
}

Edge::size_t Subdivision::addEdge(Vertex::size_t v1, Vertex::size_t v2)
{    
    return addEdge(_d->_vertices->at(v1), _d->_vertices->at(v2));
}

Edge::size_t Subdivision::addEdge(Vertex* tail, Vertex* head)
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

Edge::size_t Subdivision::edge(Vertex::size_t iidVtxTail, Vertex::size_t iidVtxHead) const
{
    assert(iidVtxTail > 0 && iidVtxHead > 0);
    
    size_t iid = 0;
    
    VtxPair pair(iidVtxTail, iidVtxHead);
    if (_d->_vtxPairEdge->find(pair) != _d->_vtxPairEdge->end()) {
        iid = _d->_vtxPairEdge->at(pair);
    }
    
    return iid;
}

Edge * Subdivision::edge(Edge::size_t iidEdge)
{
	return _d->_edges->at(iidEdge);
}

const Edge * Subdivision::edge(Edge::size_t iidEdge) const
{
	return _d->_edges->at(iidEdge);
}

Edge::size_t Subdivision::numEdges() const
{
	return static_cast<Edge::size_t>(_d->_edges->size());
}

Face::size_t Subdivision::addFace(const std::vector<Vertex::size_t>& vertexIndexList)
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

void Subdivision::replaceFace(Face::size_t /*faceIndex*/, const std::vector<Vertex::size_t>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void Subdivision::removeFace( Face::size_t iid)
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

Face* Subdivision::face(size_t iid)
{
    assert(iid > 0);
    FacesCollection::iterator it = _d->_faces->find(iid);
    return  it != _d->_faces->end() ? it->second : NULL;
}

Face::size_t Subdivision::numFaces() const
{
    assert( _d->_faces!= NULL );
    return static_cast<Face::size_t>(_d->_faces->size());
}


Face::size_t Subdivision::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

Vertex::size_t Subdivision::getClosestPointAtPoint(const Point3 & p) const
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

std::vector<Vertex::size_t> Subdivision::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //std::cerr << "Subdivision::getPointsInRadius()";
    std::vector<Vertex::size_t> results;

    NOT_IMPLEMENTED

    return results;
}

void Subdivision::lock() const
{
    NOT_IMPLEMENTED
            //_mutex.lock();
}

void Subdivision::unlock() const
{
    NOT_IMPLEMENTED
            //_mutex.unlock();
}

void Subdivision::addResolutionLevel()
{
    //TODO: Implement addResolutionLevel
    //NOT_IMPLEMENTED
    _d->subdivide_ocl(this);
//	_d->subdivide(this);
}

void Subdivision::removeResolutionLevel(int level)
{
    // TODO: Implement removeResolutionLevel
    NOT_IMPLEMENTED
}

void Subdivision::setWorkingResolutionLevel(int level)
{
    // TODO: Implement setWorkingResolutionLevel
    NOT_IMPLEMENTED
}

int Subdivision::workingResolutionLevel()
{
    // TODO: Implement getWorkingResolutionLevel
    NOT_IMPLEMENTED
    return _d->_currentResolutionLevel;
}

void Subdivision::adjustPointNormal(size_t /*index*/)
{
    NOT_IMPLEMENTED
}

Iterator<Vertex> Subdivision::vertexIterator()
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Vertex> Subdivision::constVertexIterator() const
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Face> Subdivision::faceIterator()
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Face> Subdivision::constFaceIterator() const
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Vertex> Subdivision::vertexIterator(int level)
{
    return Iterator<Vertex>(new VertexIterator(this, level) );
}

Iterator<Vertex> Subdivision::constVertexIterator(int level) const
{
    return Iterator<Vertex>(new VertexIterator(this, level) );
}

Iterator<Face> Subdivision::faceIterator(int level)
{
    return Iterator<Face>(new FaceIterator(this, level));
}

Iterator<Face> Subdivision::constFaceIterator(int level) const
{
    return Iterator<Face>(new FaceIterator(this, level));
}

Iterator<Edge> Subdivision::edgeIterator()
{
    return Iterator<Edge>(new EdgeIterator(this));
}

Iterator<Edge> Subdivision::constEdgeIterator() const
{
    return Iterator<Edge>(new EdgeIterator(this));
}

Mesh* Subdivision::convertToMesh(int level)
{
    Mesh * mesh = new ::Mesh;
    
    return mesh;
}

/**
 * Uses mesh as base mesh for this subdivision surface.
 *
 * @param mesh mesh to convert from.
 */
void Subdivision::convertFromMesh(const Mesh* mesh)
{
    
}






//////////////////////////////////////////////////////////////////////////////
// Inner classes implementation
// Subdivision::VertexIterator
Subdivision::VertexIterator::VertexIterator(const Subdivision* surface, int level)
    :	_surface(surface),
    _level(level)
{
    _index = _surface->_d->_vertices->begin();
}


IIterator<Vertex>* Subdivision::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Subdivision::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n >0 && _index != _surface->_d->_vertices->end();
}

bool Subdivision::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n > 0 &&
            (_index == _surface->_d->_vertices->end() ||
             _index != _surface->_d->_vertices->begin());
}

Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::next()
{
    Vertex *v = _index->second;
    ++_index;
    return v;
}

const Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::next() const
{
    Vertex *v = _index->second;
    ++_index;
    return v;
}

Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::peekNext() {
    return _index->second;
}

const Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::peekNext() const {
    return _index->second;
}

Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::previous()
{
    NOT_IMPLEMENTED
//    --_index;
//    return _index->second;
}

const Subdivision::VertexIterator::shared_ptr Subdivision::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
//    --_index;
//    return _index->second;
}

bool Subdivision::VertexIterator::seek(int pos, IteratorOrigin origin) const
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
        std::cerr << "Subdivision::VertexIterator::seek: not supported" << std::endl;
        return false;
//        while(++pos && _index != _surface->_d->_vertices->end()) {
//            --_index;
//        }
//        if (_index == _surface->_d->_vertices->end())
//            return false;
    }
    return true;
}

//Subdivision::FaceIterator
Subdivision::FaceIterator::FaceIterator(const Subdivision* surface, int level)
    :	_surface(surface),
    _level(level)
{
    assert(surface);
    this->tmp[0] = 'a';
    this->tmp[1] = 'b';
    _index = _surface->_d->_faces->begin();
}

IIterator<Face>* Subdivision::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Subdivision::FaceIterator::hasNext() const
{

    size_t n = _surface->numFaces();

    return n > 0 && _index != _surface->_d->_faces->end();
}

bool Subdivision::FaceIterator::hasPrevious() const
{
    size_t n = _surface->numFaces();

    return n > 0 &&
            (_index == _surface->_d->_faces->end() ||
             _index != _surface->_d->_faces->begin());
}

Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::next()
{
    Face *f = _index->second;

    assert(f);
    ++_index;
    return f;
}

const Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::next() const
{
    Face *f = _index->second;
    assert(f);
    ++_index;
    return f;
}

Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::peekNext() {
    return _index->second;
}

const Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::peekNext() const {
    return _index->second;
}


Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::previous()
{
    --_index;
    return _index->second;
}

const Subdivision::FaceIterator::shared_ptr Subdivision::FaceIterator::previous() const
{
    --_index;
    return _index->second;
}

bool Subdivision::FaceIterator::seek(int pos, IteratorOrigin origin) const
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

void Subdivision::printMemoryInfo() const
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
Subdivision::Impl::initialize_ocl(void)
{
	cl_int err = CL_SUCCESS;
	
	try {
		CLManager * oclManager = CLManager::instance();
		
		std::string kernelSource = opencl::loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/testapp/Subdivision.cl");
		cl::Program::Sources source(1,
									std::make_pair(kernelSource.c_str(),kernelSource.length()));
		program = cl::Program(oclManager->context(), source);
		program.build(oclManager->devices());
		
		krnInit = cl::Kernel(program,"surface_init",&err);
		krnSubdivideEdges = cl::Kernel(program, "subdivide_edges_midpoint", &err);
		krnSubdivideFaces = cl::Kernel(program, "subdivide_faces_midpoint", &err);
		krnSubdivideAddFaces = cl::Kernel(program, "subdivide_add_faces", &err);
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
	
	return EXIT_SUCCESS;
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

struct clSurfaceList
Subdivision::Impl::create_from_surface(Subdivision * surface) {
	struct clSurfaceList ret;
	
	if (!surface) {
		ret.num_polygons = 0;
		return ret;
	}
	
	ret.vertex.reserve(1000);
	auto vtxIt = surface->constVertexIterator();
	while (vtxIt.hasNext()) {
		auto v = vtxIt.next();
		if (!v) {
			throw std::runtime_error("Invalid vertex in surface");
		}
		
		struct clVertex vtx;
		vtx.p.s[0] = v->position()[0];
		vtx.p.s[1] = v->position()[1];
		vtx.p.s[2] = v->position()[2];
		vtx.p.s[3] = 1;
		vtx.n.s[0] = v->normal()[0];
		vtx.n.s[1] = v->normal()[1];
		vtx.n.s[2] = v->normal()[2];
		vtx.n.s[3] = 0;
		ret.vertex.push_back(vtx);
		hndVtxIndex[v->iid()] = uint(ret.vertex.size()) - 1;
	}
	
	ret.edge.reserve(1000);
	auto edgeIt = surface->constEdgeIterator();
	while (edgeIt.hasNext()) {
		auto e = edgeIt.next();
		if (!e) {
			throw std::runtime_error("Invalid edge in surface");
		}
		if (hndEdgeIndex.find(e->iid()) != hndEdgeIndex.end()) {
			continue;
		}
		struct clEdge edge1, edge2;
		ret.edge.push_back(edge1);
		ret.edge.push_back(edge2);
		hndEdgeIndex[e->iid()] = uint(ret.edge.size()) - 2;
		hndEdgeIndex[e->pair()->iid()] = uint(ret.edge.size()) - 1;
	}
	edgeIt = surface->constEdgeIterator();
	while (edgeIt.hasNext()) {
		auto e = edgeIt.next();
		if (!e) {
			throw std::runtime_error("Invalid edge in surface");
		}
		uint index = hndEdgeIndex[e->iid()];
		ret.edge[index].head = hndVtxIndex[e->head()->iid()];
		ret.edge[index].next = hndEdgeIndex[e->next()->iid()];
		ret.edge[index].pair = hndEdgeIndex[e->pair()->iid()];
	}
	
	ret.face.reserve(1000);
	auto faceIt = surface->constFaceIterator();
	while(faceIt.hasNext()){
		auto f = faceIt.next();
		if (!f) {
			throw std::runtime_error("Invalid face in surface");
		}
		struct clFace face;
		face.num_edges = f->numVertices();
		face.edge = hndEdgeIndex[f->hedge()->iid()];
		face.vertex = 0;
		ret.face.push_back(face);
		uint faceIndex = uint(ret.face.size()-1);
		hndFaceIndex[f->iid()] = faceIndex;
		uint edgeIndex = hndEdgeIndex[f->hedge()->iid()];
		ret.edge[edgeIndex].face = faceIndex;
	}
	
	return ret;
}

void
Subdivision::Impl::update_surface_from_polygon(Subdivision * surface,
							clSurfaceList & list)
{
	if (!surface) {
		return;
	}
	
	_vertices = new VertexCollection;
    _edges = new EdgesCollection;
    _vtxPairEdge = new VtxPairEdgeMap;
    _faces = new FacesCollection;
	
    _vertices->reserve(50000);
    _edges->reserve(50000);
    _vtxPairEdge->reserve(50000);
    
    _vertLevelCollections.push_back(_vertices);
    _edgesLevelCollections.push_back(_edges);
    _vertexEdgeCollection.push_back(_vtxPairEdge);
    _facesLevelCollections.push_back(_faces);
	
	std::vector<uint> vtxIdxIid(hndVtxIndex.size());
		
	for (uint i = 0; i < list.vertex.size(); ++i) {
		Point3 p(list.vertex[i].p.s[0],list.vertex[i].p.s[1],list.vertex[i].p.s[2]);
		Vertex::size_t iid = surface->addVertex(p);
		hndVtxIndex[iid] = i;
		vtxIdxIid[i] = iid;
		std::cout << "Point[" << i << "]=" << toString(p) << std::endl;
	}
	//surface->vertex()
	
	std::vector<size_t> indexList;
	indexList.reserve(10);
	for (uint i = 0; i < list.face.size(); ++i) {
		indexList.clear();
		clEdge *e0 = &list.edge[list.face[i].edge];
		clEdge *e = e0;
		do {
			indexList.push_back(vtxIdxIid[e->head]);
			e = &list.edge[e->next];
		} while (e != e0);
		surface->addFace( indexList );
	}

	surface->setChanged(true);
}

void Subdivision::Impl::subdivide_ocl(Subdivision * s)
{
	if (!oclInitialized) {
		initialize_ocl();
		oclInitialized = true;
	}
	
	
	try {
		CLManager * oclManager = CLManager::instance();
		cl::Context context = oclManager->context();
		cl::CommandQueue queue = oclManager->commandQueue();
		
		std::cout << "sizeof(clVertex): " << sizeof(clVertex) << std::endl;
		std::cout << "sizeof(clEdge): " << sizeof(clEdge) << std::endl;
		std::cout << "sizeof(clFace): " << sizeof(clFace) << std::endl;
		std::cout << "sizeof(clSurface): " << sizeof(clSurface) << std::endl;
		
		clSurfaceList surface1 = create_from_surface(s);
		std::cout << "Vertex size: " << surface1.vertex.size() << std::endl;
		std::cout << "Edge size: " << surface1.edge.size() << std::endl;
		std::cout << "Face size: " << surface1.face.size() << std::endl;
		
		cl::Buffer surfaceBuffer1(context,CL_MEM_READ_WRITE, sizeof(clSurface), 0);
		
		unsigned long vertexBufferSize1 = surface1.vertex.capacity()*sizeof(clVertex);
		cl::Buffer vertexBuffer1(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vertexBufferSize1,surface1.vertex.data());
		
		unsigned long edgeBufferSize1 = surface1.edge.capacity()*sizeof(clEdge);
		cl::Buffer edgeBuffer1(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, edgeBufferSize1,surface1.edge.data());
		
		unsigned long faceBufferSize1 = surface1.face.capacity()*sizeof(clFace);
		cl::Buffer faceBuffer1(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, faceBufferSize1,surface1.face.data());
		
		clSurfaceList surface2 = create_from_surface(s);
		
		cl::Buffer surfaceBuffer2(context,CL_MEM_READ_WRITE, sizeof(clSurface), 0);
		
		unsigned long vertexBufferSize2 = surface2.vertex.capacity()*sizeof(clVertex);
		cl::Buffer vertexBuffer2(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vertexBufferSize2,surface2.vertex.data());
		
		unsigned long edgeBufferSize2 = surface2.edge.capacity()*sizeof(clEdge);
		cl::Buffer edgeBuffer2(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, edgeBufferSize2,surface2.edge.data());
		
		unsigned long faceBufferSize2 = surface2.face.capacity()*sizeof(clFace);
		cl::Buffer faceBuffer2(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, faceBufferSize2,surface2.face.data());
		
		krnInit.setArg(0, surfaceBuffer1);
		krnInit.setArg(1, vertexBuffer1);
		krnInit.setArg(2, surface1.vertex.size());
		krnInit.setArg(3, edgeBuffer1);
		krnInit.setArg(4, surface1.edge.size());
		krnInit.setArg(5, faceBuffer1);
		krnInit.setArg(6, surface1.face.size());
		queue.enqueueTask(krnInit,NULL,NULL);
		
		krnInit.setArg(0, surfaceBuffer2);
		krnInit.setArg(1, vertexBuffer2);
		krnInit.setArg(2, surface2.vertex.size());
		krnInit.setArg(3, edgeBuffer2);
		krnInit.setArg(4, surface2.edge.size());
		krnInit.setArg(5, faceBuffer2);
		krnInit.setArg(6, surface2.face.size());
		cl::Event evtInit;
		queue.enqueueTask(krnInit,NULL,&evtInit);
		
		krnSubdivideEdges.setArg(0, surfaceBuffer1);
		krnSubdivideEdges.setArg(1, surfaceBuffer2);
		
		std::vector<cl::Event> evtLst;
		evtLst.push_back(evtInit);
		cl::Event event;
		queue.enqueueNDRangeKernel(
								   krnSubdivideEdges,
								   cl::NullRange,
								   cl::NDRange(s->numEdges()/2),
								   cl::NullRange,
								   &evtLst,
								   &event);
		clSurface surface;
		queue.enqueueReadBuffer(surfaceBuffer2, CL_TRUE, 0, sizeof(clSurface), &surface);
		std::cout << "Num Vertices: " << surface.num_vertices << std::endl;
		std::cout << "Num Faces: " << surface.num_faces << std::endl;
		
		krnSubdivideFaces.setArg(0, surfaceBuffer1);
		krnSubdivideFaces.setArg(1, surfaceBuffer2);
		queue.enqueueNDRangeKernel(
								   krnSubdivideFaces,
								   cl::NullRange,
								   cl::NDRange(s->numFaces()),
								   cl::NullRange,
								   &evtLst,
								   &event);
		
		queue.enqueueReadBuffer(surfaceBuffer2, CL_TRUE, 0, sizeof(clSurface), &surface);
		std::cout << "Num Vertices: " << surface.num_vertices << std::endl;
		std::cout << "Num Faces: " << surface.num_faces << std::endl;
		
		krnSubdivideAddFaces.setArg(0, surfaceBuffer1);
		krnSubdivideAddFaces.setArg(1, surfaceBuffer2);
		queue.enqueueNDRangeKernel(
								   krnSubdivideAddFaces,
								   cl::NullRange,
								   cl::NDRange(s->numFaces()),
								   cl::NullRange,
								   &evtLst,
								   &event);
		
		event.wait();
		queue.enqueueReadBuffer(surfaceBuffer2, CL_TRUE, 0, sizeof(clSurface), &surface);
		std::cout << "Num Vertices: " << surface.num_vertices << std::endl;
		std::cout << "Num Faces: " << surface.num_faces << std::endl;
		surface2.vertex.resize(surface.num_vertices);
		vertexBufferSize2 = surface2.vertex.size()*sizeof(clVertex);
		queue.enqueueReadBuffer(vertexBuffer2, CL_TRUE, 0, vertexBufferSize2, surface2.vertex.data());
		
		
		update_surface_from_polygon(s, surface2);
		
//		BOManager::getInstance()->invalidateBO(s);
		
	} catch (cl::Error err) {
		std::cerr
		<< "ERROR: "
		<< err.what()
		<< "("
		<< err.err()
		<< ")"
		<< std::endl;
	}
}
