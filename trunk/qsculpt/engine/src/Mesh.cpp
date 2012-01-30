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
#include "StdAfx.h"
#include "Mesh.h"

#include <QtOpenGL>
#include <QVector>
#include <math.h>
#include <Eigen/Core>
#include <map>

#include "Point3D.h"
#include "Scene.h"
#include "HEdge.h"
#include "geometry/Aabb.h"
#include "Color.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

typedef std::pair<int, int>     VtxPair;
typedef QHash<int, Vertex*>     VertexCollection;
typedef QHash<int, Edge*>       EdgesCollection;
typedef QHash<VtxPair, int>     VtxPairEdgeMap;
typedef std::map<int, Face*>       FacesCollection;


struct VertexPool {
    std::vector<size_t> free;
    std::vector<Vertex> element;
};

static QAtomicInt NEXT_ID;

struct Mesh::Impl {
    /** Instance ID of the surface */
    uint _iid;
    
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
    QVector<int>    _selectedPoints;
    
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

// Iterator classes declarations
class Mesh::VertexIterator : public IIterator<Vertex>
{	
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
    Vertex & next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const Vertex & next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    Vertex & peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const Vertex & peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    Vertex & previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const Vertex & previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class Mesh::FaceIterator : public IIterator<Face>
{
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
    Face & next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const Face & next() const;
    
    /**
     * Returns the next element without advancing to the next
     */
    Face & peekNext();
    
    /**
     * Returns the next element without advancing to the next
     */
    const Face & peekNext() const ;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    Face & previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const Face & previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const ;
};

class Mesh::EdgeIterator : public IIterator<Edge>
{
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
    Edge & next() {
        Edge * e = index_.value();
        ++index_;
        return *e;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const Edge & next() const {
        Edge * e = index_.value();
        ++index_;
        return *e;
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    Edge & peekNext() {
        return *index_.value();
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const Edge & peekNext() const {
        return *index_.value();
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    Edge & previous() {
        NOT_IMPLEMENTED
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const Edge & previous() const {
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
    _d->_iid = NEXT_ID.fetchAndAddRelaxed(1);
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

uint Mesh::iid() const
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
        emit meshChanged(this);
    }
}

QVector<int> Mesh::selectedPoints() const {
    return _d->_selectedPoints;
}

void Mesh::setSelectedPoints(const QVector<int>& indicesArray) {
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

int Mesh::addVertex(const Point3& point)
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

int Mesh::addVertex(Vertex* v)
{
    _d->_boundingBox.extend(v->position());
    this->_d->_vertices->insert(v->iid(), v);
    return v->iid();
}

void Mesh::removeVertex(int iid)
{
    _d->_vertices->remove(iid);
}

Vertex* Mesh::vertex(int iid)
{
    return _d->_vertices->contains(iid) ? _d->_vertices->value(iid) : NULL;
}

const Vertex* Mesh::vertex(int iid) const
{
    return _d->_vertices->contains(iid) ? _d->_vertices->value(iid) : NULL;
}

int Mesh::numVertices() const
{
    return _d->_vertices->size();
}

int Mesh::addEdge(const Edge& edge)
{
    return addEdge(edge.tail(), edge.head());
}

int Mesh::addEdge(int v1, int v2)
{    
    return addEdge(_d->_vertices->value(v1), _d->_vertices->value(v2));
}

int Mesh::addEdge(Vertex* tail, Vertex* head)
{
    assert(tail && head);
    
    int iid = -1;
    
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

int Mesh::addFace(const QVector<int>& vertexIndexList)
{
    //NOT_IMPLEMENTED
    std::vector<Edge*> edges;
    int size = vertexIndexList.size();
    if (size < 3) {
        qWarning() << "addFace: not enough vertices: " << size;
        return -1;
    }
    
    for (int i = 0; i < size; ++i) {
        int iid = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
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

void Mesh::replaceFace(int /*faceIndex*/, const QVector<int>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}


// clear reference to face from edges and remove the face from this surface.
void Mesh::removeFace( int iid)
{
    Face * f = face(iid);
    if (f == NULL)
        return;
    Iterator<Edge> edgeIt = f->edgeIterator();
    while (edgeIt.hasNext()) {
        Edge *e = &edgeIt.next();
        e->setFace(NULL);
        e->setNext(NULL);
    }
    _d->_faces->erase(iid);
}

Face* Mesh::face(int iid)
{
    assert(iid > 0);
    FacesCollection::iterator it = _d->_faces->find(iid);
    return  it != _d->_faces->end() ? it->second : NULL;
}

int Mesh::numFaces() const
{
    assert( _d->_faces!= NULL );
    return _d->_faces->size();
}


int Mesh::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

int Mesh::getClosestPointAtPoint(const Point3 &/*p*/) const
{
    NOT_IMPLEMENTED
    
    return -1;
}

QVector<int> Mesh::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //qDebug() << "Mesh::getPointsInRadius()";
    QVector<int> results;
    
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

void Mesh::adjustPointNormal(int /*index*/)
{
    NOT_IMPLEMENTED
}

Iterator<Vertex> Mesh::vertexIterator()
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Vertex> Mesh::constVertexIterator() const
{
    return Iterator<Vertex>(new VertexIterator(this) );
}

Iterator<Face> Mesh::faceIterator()
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Face> Mesh::constFaceIterator() const
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Edge> Mesh::edgeIterator()
{
    return Iterator<Edge>(new EdgeIterator(this));
}

Iterator<Edge> Mesh::constEdgeIterator() const
{
    return Iterator<Edge>(new EdgeIterator(this));
}

// Inner classes implementation
// Mesh::VertexIterator
Mesh::VertexIterator::VertexIterator(const Mesh* surface, int level)
:	_surface(surface),
_level(level)
{
    _index = _surface->_d->_vertices->begin();
}


IIterator<Vertex>* Mesh::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Mesh::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    int n = _surface->numVertices();
    return n >0 && _index != _surface->_d->_vertices->end();
}

bool Mesh::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    int n = _surface->numVertices();
    return n > 0 &&
    (_index == _surface->_d->_vertices->end() ||
     _index != _surface->_d->_vertices->begin());
}

Vertex & Mesh::VertexIterator::next()
{
    Vertex *v = _index.value();
    ++_index;
    return *v;
}

const Vertex & Mesh::VertexIterator::next() const
{
    Vertex *v = _index.value();
    ++_index;
    return *v;
}

Vertex & Mesh::VertexIterator::peekNext() {
    return *_index.value();
}

const Vertex & Mesh::VertexIterator::peekNext() const {
    return *_index.value();
}

Vertex & Mesh::VertexIterator::previous()
{
    --_index;
    return *_index.value();
}

const Vertex & Mesh::VertexIterator::previous() const
{
    --_index;
    return *_index.value();
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
        while(++pos && _index != _surface->_d->_vertices->end()) {
            --_index;
        }
        if (_index == _surface->_d->_vertices->end())
            return false;
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

IIterator<Face>* Mesh::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Mesh::FaceIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    int n = _surface->numFaces();
    //    return n > 0 && _index < n-1;
    return n > 0 && _index != _surface->_d->_faces->end();
}

bool Mesh::FaceIterator::hasPrevious() const
{
    int n = _surface->numFaces();
    //    return n > 0 && _index < n && _index >= 0;
    return n > 0 &&
    (_index == _surface->_d->_faces->end() ||
     _index != _surface->_d->_faces->begin());
}

Face & Mesh::FaceIterator::next()
{
    //NOT_IMPLEMENTED
    //    return *_surface->_faces->at(++_index);
    Face *f = _index->second;
    
    assert(f);
    ++_index;
    return *f;
}

const Face & Mesh::FaceIterator::next() const
{
    //NOT_IMPLEMENTED
    //    assert(_surface->getNumFaces() > _index);
    //    return *_surface->_faces->at(++_index);
    Face *f = _index->second;
    assert(f);
    ++_index;
    return *f;
}

Face & Mesh::FaceIterator::peekNext() {
    return *_index->second;
}

const Face & Mesh::FaceIterator::peekNext() const {
    return *_index->second;
}


Face & Mesh::FaceIterator::previous()
{
    //    assert(_index >= 0);
    //    Face* f = _surface->_faces->at(_index);
    //    --_index;
    //    return *f;
    --_index;
    return *_index->second;
}

const Face & Mesh::FaceIterator::previous() const
{
    //    assert(_index >= 0);
    //    Face* f = _surface->_faces->at(_index);
    //    --_index;
    //    return *f;
    --_index;
    return *_index->second;
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
    unsigned int sizeVertex, sizeEdge, sizeFace;
    unsigned int nVertex, nFace, nEdge;
    
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



