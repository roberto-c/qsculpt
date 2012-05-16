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
#include "subdivision/Subdivision.h"

#include <QtOpenGL/QtOpenGL>
#include <QtCore/QVector>
#include <math.h>
#include <Eigen/Core>
#include <map>

#include "subdivision/Box.h"
#include "Point3D.h"
#include "Scene.h"
#include "HEdge.h"
#include "geometry/Aabb.h"
#include "Color.h"
#include "Mesh.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

typedef std::pair<size_t, size_t>     VtxPair;
typedef QHash<size_t, Vertex*>     VertexCollection;
typedef QHash<size_t, Edge*>       EdgesCollection;
typedef QHash<VtxPair, size_t>     VtxPairEdgeMap;
typedef std::map<size_t, Face*>       FacesCollection;


struct VertexPool {
    std::vector<size_t> free;
    std::vector<Vertex> element;
};

static QAtomicInt NEXT_ID;

struct Subdivision::Impl {
    /** Instance ID of the surface */
    size_t _iid;
    
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
    QVector<size_t>    _selectedPoints;
    
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
    _currentResolutionLevel(0),
    _hasChanged(true)
    {
        
    }
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
    SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    SharedPtr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const SharedPtr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const;

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
    SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    SharedPtr peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const SharedPtr peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const;

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
        return (index_ != surface_->_d->_edges->end()) && (index_.value() != NULL);
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
    SharedPtr next() {
        Edge * e = index_.value();
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const SharedPtr next() const {
        Edge * e = index_.value();
        ++index_;
        return e;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    SharedPtr peekNext() {
        return index_.value();
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const SharedPtr peekNext() const {
        return index_.value();
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    SharedPtr previous() {
        NOT_IMPLEMENTED
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const SharedPtr previous() const {
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

Subdivision::Subdivision()
    :   ISurface(),
    _d(new Impl)
{
    _d->_iid = NEXT_ID.fetchAndAddRelaxed(1);
    initPoints();
    //updateBoundingBox();
}

Subdivision::Subdivision(const Mesh * mesh)
: ISurface(),
_d(new Impl)
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

size_t Subdivision::iid() const
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
        emit meshChanged(this);
    }
}

QVector<size_t> Subdivision::selectedPoints() const {
    return _d->_selectedPoints;
}

void Subdivision::setSelectedPoints(const QVector<size_t>& indicesArray) {
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

const geometry::AABB& Subdivision::boundingBox() const
{
    return _d->_boundingBox;
}

void Subdivision::setColor(const Color& color)
{
    _d->_color = color;
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

size_t Subdivision::addVertex(const Point3& point)
{
    assert(_d->_vertices != NULL );
    //qWarning("%s %s", __FUNCTION__, " Not implemented");
    
    Vertex* vertex = new Vertex(point);
//    size_t index = _d->_vtxPool.free[0];
//    _d->_vtxPool.free.pop_back();
//    Vertex* vertex = &_d->_vtxPool.element.at(index);
//    vertex->position() = point;
    _d->_boundingBox.extend(point);
    this->_d->_vertices->insert(vertex->iid(), vertex);
    return vertex->iid();

}

size_t Subdivision::addVertex(Vertex* v)
{
    _d->_boundingBox.extend(v->position());
    this->_d->_vertices->insert(v->iid(), v);
    return v->iid();
}

void Subdivision::removeVertex(size_t iid)
{
    _d->_vertices->remove(iid);
}

Vertex* Subdivision::vertex(size_t iid)
{
    return _d->_vertices->contains(iid) ? _d->_vertices->value(iid) : NULL;
}

const Vertex* Subdivision::vertex(size_t iid) const
{
    return _d->_vertices->contains(iid) ? _d->_vertices->value(iid) : NULL;
}

size_t Subdivision::numVertices() const
{
    return _d->_vertices->size();
}

size_t Subdivision::addEdge(const Edge& edge)
{
    return addEdge(edge.tail(), edge.head());
}

size_t Subdivision::addEdge(size_t v1, size_t v2)
{    
    return addEdge(_d->_vertices->value(v1), _d->_vertices->value(v2));
}

size_t Subdivision::addEdge(Vertex* tail, Vertex* head)
{
    assert(tail && head);
    
    size_t iid = -1;

    VtxPair pair(tail->iid(), head->iid());
    if (_d->_vtxPairEdge->contains(pair)) {
        iid = _d->_vtxPairEdge->value(pair);
    } else {
        Edge *h1 = new Edge(tail, head);
        Edge *h2 = new Edge(head, tail);
        h1->setPair(h2);
        h2->setPair(h1);

        h1->head()->setEdge(h2);

        iid = h1->iid();
        _d->_edges->insert(h1->iid(), h1);
        _d->_edges->insert(h2->iid(), h2);
        _d->_vtxPairEdge->insert(pair, h1->iid());
        pair.first = head->iid();
        pair.second = tail->iid();
        _d->_vtxPairEdge->insert(pair, h2->iid());
        
        if (tail->isFlagSet(VF_Crease) && head->isFlagSet(VF_Crease)) {
            h1->addFlag(EF_Crease);
            h2->addFlag(EF_Crease);
        }
    }

    return iid;
}

size_t Subdivision::addFace(const QVector<size_t>& vertexIndexList)
{
    //NOT_IMPLEMENTED
    std::vector<Edge*> edges;
    int size = vertexIndexList.size();
    if (size < 3) {
        qWarning() << "addFace: not enough vertices: " << size;
        return -1;
    }
    
    for (int i = 0; i < size; ++i) {
        size_t iid = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
        assert(iid > 0);
        Edge *e = _d->_edges->value(iid);
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

void Subdivision::replaceFace(size_t /*faceIndex*/, const QVector<size_t>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void Subdivision::removeFace( size_t iid)
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

size_t Subdivision::numFaces() const
{
    assert( _d->_faces!= NULL );
    return _d->_faces->size();
}


size_t Subdivision::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

size_t Subdivision::getClosestPointAtPoint(const Point3 & p) const
{
    float d = MAXFLOAT;
    Vertex::SharedPtr vtx = nullptr, tmpVtx = nullptr;
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

QVector<size_t> Subdivision::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //qDebug() << "Subdivision::getPointsInRadius()";
    QVector<size_t> results;

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
    NOT_IMPLEMENTED
}

void Subdivision::removeResolutionLevel(int level)
{
    // TODO: Implement removeResolutionLevel
    Q_UNUSED(level);
    NOT_IMPLEMENTED
}

void Subdivision::setWorkingResolutionLevel(int level)
{
    // TODO: Implement setWorkingResolutionLevel
    Q_UNUSED(level);
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

Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::next()
{
    Vertex *v = _index.value();
    ++_index;
    return v;
}

const Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::next() const
{
    Vertex *v = _index.value();
    ++_index;
    return v;
}

Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::peekNext() {
    return _index.value();
}

const Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::peekNext() const {
    return _index.value();
}

Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::previous()
{
    --_index;
    return _index.value();
}

const Subdivision::VertexIterator::SharedPtr Subdivision::VertexIterator::previous() const
{
    --_index;
    return _index.value();
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
        while(++pos && _index != _surface->_d->_vertices->end()) {
            --_index;
        }
        if (_index == _surface->_d->_vertices->end())
            return false;
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

Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::next()
{
    Face *f = _index->second;

    assert(f);
    ++_index;
    return f;
}

const Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::next() const
{
    Face *f = _index->second;
    assert(f);
    ++_index;
    return f;
}

Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::peekNext() {
    return _index->second;
}

const Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::peekNext() const {
    return _index->second;
}


Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::previous()
{
    --_index;
    return _index->second;
}

const Subdivision::FaceIterator::SharedPtr Subdivision::FaceIterator::previous() const
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
    size_t sizeVertex, sizeEdge, sizeFace;
    size_t nVertex, nFace, nEdge;
    
    sizeVertex = sizeof(Vertex);
    sizeFace = sizeof(Face);
    sizeEdge = sizeof(Edge);
    nVertex = this->numVertices();
    nFace = this->numFaces();
    nEdge = this->_d->_edges->size();
    
    qDebug() << "Size of vertex: " << sizeVertex;
    qDebug() << "Size of Face: " << sizeFace;
    qDebug() << "Size of Edge: " << sizeEdge;
    qDebug() << "Num verts: " << nVertex;
    qDebug() << "Num facess: " << nFace;
    qDebug() << "Num edges: " << nEdge;
    
    qDebug() << "Size of vertex hash: " << sizeof(*_d->_vertices);
    
    qDebug() << "Mem usage: " << (sizeVertex * nVertex) 
        + (sizeEdge * nEdge) 
        + (sizeFace * nFace)
        + sizeof(*_d->_vertices);
}
