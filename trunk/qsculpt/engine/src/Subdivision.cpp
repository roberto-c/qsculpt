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
#include "Subdivision.h"

#include <QColor>
#include <QtOpenGL>
#include <QVector>
#include <math.h>
#include <Eigen/Core>

#include "Box.h"
#include "Point3D.h"
#include "Scene.h"
#include "HEdge.h"
#include "Aabb.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

// Iterator classes declarations
class Subdivision::VertexIterator : public IIterator<Vertex>
{	
    friend class Subdivision;
public:
    typedef Subdivision::VertexCollection::iterator iterator;

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
    Vertex & next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const Vertex & next() const;
    
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

class Subdivision::FaceIterator : public IIterator<Face>
{
    friend class Subdivision;

    typedef Subdivision::FacesCollection::iterator iterator;

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
    Face & next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const Face & next() const;
    
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



Subdivision::Subdivision()
    :   ISurface(),
    _vertices(NULL),
    _edges(NULL),
    _vtxPairEdge(NULL),
    _faces(NULL),
    m_scene(NULL),
    //m_drawingMode(Wireframe),
    m_color(Qt::white),
    m_rotX(0.0),
    m_rotY(0.0),
    m_rotZ(0.0),
    m_selected(false),
    m_callListId(0),
    m_genereateCallList(true),
    m_currentResolutionLevel(0),
    m_hasChanged(true)
{
    initPoints();
    //updateBoundingBox();
}

Subdivision::~Subdivision()
{
    _vertices = NULL;
    _edges = NULL;
    _vtxPairEdge = NULL;
    _faces = NULL;

    std::vector<VertexCollection*>::iterator it = _vertLevelCollections.begin();
    for (; it != _vertLevelCollections.end(); ++it)
    {
        _vertices = (*it);
        //Vertex
    }
}

void Subdivision::initPoints()
{
    _vertices = new VertexCollection;
    _edges = new EdgesCollection;
    _vtxPairEdge = new VtxPairEdgeMap;
    _faces = new FacesCollection;

    _vertLevelCollections.push_back(_vertices);
    _edgesLevelCollections.push_back(_edges);
    _vertexEdgeCollection.push_back(_vtxPairEdge);
    _facesLevelCollections.push_back(_faces);
}

bool Subdivision::hasChanged() {
    return m_hasChanged;
}

void Subdivision::setChanged(bool val) {
    m_hasChanged = val;
    if (val) {
        emit meshChanged(this);
    }
}

QVector<int> Subdivision::getSelectedPoints() const {
    return m_selectedPoints;
}

void Subdivision::setSelectedPoints(const QVector<int>& indicesArray) {
    m_selectedPoints = indicesArray;
}

void Subdivision::setScene(Scene* scene)
{
    m_scene = scene;
}

Scene* Subdivision::getScene() const
{
    return m_scene;
}

Point3 Subdivision::getPosition() const
{
    return m_position;
}

void Subdivision::displace(const Point3& delta)
{
    m_position = m_position + delta;
    emit positionChanged(m_position.x(), m_position.y(), m_position.z());
}

void Subdivision::getPosition(float *x, float *y, float *z) const
{
    if (x) *x = m_position.x();
    if (y) *y = m_position.y();
    if (z) *z = m_position.z();
}

void Subdivision::rotate(float rotX, float rotY, float rotZ)
{
    m_rotX = rotX;
    m_rotY = rotY;
    m_rotZ = rotZ;
}

void Subdivision::setPosition(float x, float y, float z)
{
    m_position.x() = x;
    m_position.y() = y;
    m_position.z() = z;

    emit positionChanged(x, y, z);
}

void Subdivision::setPosition(const Point3& position)
{
    m_position = position;
    emit positionChanged(m_position.x(), m_position.y(), m_position.z());
}

const geometry::AABB& Subdivision::getBoundingBox() const
{
    return _boundingBox;
}

void Subdivision::setColor(const QColor& color)
{
    m_color = color;
}

QColor Subdivision::getColor() const
{
    return m_color;
}

void Subdivision::setSelected(bool val)
{
    m_selected = val;
}

bool Subdivision::isSelected() const
{
    return m_selected;
}

int Subdivision::addVertex(const Point3& point)
{
    assert(_vertices != NULL );
    //qWarning("%s %s", __FUNCTION__, " Not implemented");
    _boundingBox.extend(point);

    Vertex* vertex = new Vertex(point);
//    this->_vertices->push_back(vertex);
//    return _vertices->size() - 1;
    this->_vertices->insert(vertex->iid(), vertex);
    return vertex->iid();

}

int Subdivision::addVertex(Vertex* v)
{
    _boundingBox.extend(v->getPosition());
    this->_vertices->insert(v->iid(), v);
    return v->iid();
}

void Subdivision::removeVertex(int iid)
{
    _vertices->remove(iid);
}

Vertex* Subdivision::getVertex(int iid)
{
    return _vertices->contains(iid) ? _vertices->value(iid) : NULL;
}

const Vertex* Subdivision::getVertex(int iid) const
{
    return _vertices->contains(iid) ? _vertices->value(iid) : NULL;
}

int Subdivision::getNumVertices() const
{
    return _vertices->size();
}

int Subdivision::addEdge(const Edge& edge)
{
    return addEdge(edge.tail(), edge.head());
}

int Subdivision::addEdge(int v1, int v2)
{    
    return addEdge(_vertices->value(v1), _vertices->value(v2));
}

int Subdivision::addEdge(Vertex* tail, Vertex* head)
{
    int iid = -1;

    VtxPair pair(tail->iid(), head->iid());
    if (_vtxPairEdge->contains(pair)) {
        iid = _vtxPairEdge->value(pair);
    } else {
        Edge *h1 = new Edge(tail, head);
        Edge *h2 = new Edge(head, tail);
        h1->setPair(h2);
        h2->setPair(h1);

        h1->head()->setEdge(h2);

        iid = h1->iid();
        _edges->insert(h1->iid(), h1);
        _edges->insert(h2->iid(), h2);
        _vtxPairEdge->insert(pair, h1->iid());
        pair.first = head->iid();
        pair.second = tail->iid();
        _vtxPairEdge->insert(pair, h2->iid());
    }

    return iid;
}

int Subdivision::addFace(const QVector<int>& vertexIndexList)
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
        edges.push_back(_edges->value(iid));
    }
    Face *f = new Face(this);
    f->setHEdge(edges[0]);
    for (int i = 0; i < size; ++i) {
        edges[i]->setNext(edges[(i+1)%size]);
        edges[i]->setFace(f);
        //_edges[edgesIndices[i]].setNext(edgesIndices[(i+1)%size]);
    }
    _faces->insert(f->iid(), f);
    return f->iid();
}

void Subdivision::replaceFace(int /*faceIndex*/, const QVector<int>& /*vertexIndexList*/)
{
    NOT_IMPLEMENTED
}

void Subdivision::removeFace( int iid)
{
//    Face* f = NULL;
//    if (_faces->contains(iid))
//        f = _faces->value(iid);
//    if (f) {
//        f->addFlag(FF_Deleted);
//        Edge *e = f->hedge();
//        do {
//            e->setFace(NULL);
//            e = e->next();
//            // TODO: Check edges for deletion?
//        }while(e != f->hedge());
//    }
    _faces->remove(iid);
}

Face* Subdivision::getFace(int iid)
{
    assert(iid > 0 && iid < _faces->size());
    return _faces->contains(iid) ? _faces->value(iid) : NULL;
}

int Subdivision::getNumFaces() const
{
    assert( _faces!= NULL );
    return _faces->size();
}

//void Subdivision::drawBoundingBox()
//{
//    GLboolean lightEnabled = GL_FALSE;
//    lightEnabled = glIsEnabled(GL_LIGHTING);
//
//    if (lightEnabled == GL_TRUE)
//        glDisable(GL_LIGHTING);
//
//    glColor3f(0.0, 1.0, 0.0);
//
//    float minX = m_minX - 0.1;
//    float maxX = m_maxX + 0.1;
//    float minY = m_minY - 0.1;
//    float maxY = m_maxY + 0.1;
//    float minZ = m_minZ - 0.1;
//    float maxZ = m_maxZ + 0.1;
//
//    glColor3f(0.0, 1.0, 0.0);
//
//    glBegin(GL_LINE_LOOP);
//    glVertex3f(minX, minY, minZ);
//    glVertex3f(maxX, minY, minZ);
//    glVertex3f(maxX, maxY, minZ);
//    glVertex3f(minX, maxY, minZ);
//    glEnd();
//    glBegin(GL_LINE_LOOP);
//    glVertex3f(minX, minY, maxZ);
//    glVertex3f(maxX, minY, maxZ);
//    glVertex3f(maxX, maxY, maxZ);
//    glVertex3f(minX, maxY, maxZ);
//    glEnd();
//
//    glBegin(GL_LINES);
//    glVertex3f(minX, minY, minZ);
//    glVertex3f(minX, minY, maxZ);
//
//    glVertex3f(maxX, minY, minZ);
//    glVertex3f(maxX, minY, maxZ);
//
//    glVertex3f(maxX, maxY, minZ);
//    glVertex3f(maxX, maxY, maxZ);
//
//    glVertex3f(minX, maxY, minZ);
//    glVertex3f(minX, maxY, maxZ);
//    glEnd();
//
//    if (lightEnabled == GL_TRUE)
//        glEnable(GL_LIGHTING);
//}

int Subdivision::getFaceIndexAtPoint(const Point3& /*p*/) const
{
    NOT_IMPLEMENTED
    return -1;
}

int Subdivision::getClosestPointAtPoint(const Point3 &/*p*/) const
{
    NOT_IMPLEMENTED

    return -1;
}

QVector<int> Subdivision::getPointsInRadius(const Point3 &/*p*/, float /*radius*/) const
{
    //qDebug() << "Subdivision::getPointsInRadius()";
    QVector<int> results;

    NOT_IMPLEMENTED

    return results;
}

void Subdivision::lock() const
{
    NOT_IMPLEMENTED
            //m_mutex.lock();
}

void Subdivision::unlock() const
{
    NOT_IMPLEMENTED
            //m_mutex.unlock();
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

int Subdivision::getWorkingResolutionLevel()
{
    // TODO: Implement getWorkingResolutionLevel
    NOT_IMPLEMENTED
    return m_currentResolutionLevel;
}

void Subdivision::adjustPointNormal(int /*index*/)
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

Point3 Subdivision::localToWorldCoords(const Point3& p) const
{
    Eigen::Matrix3f m;
    m = Eigen::AngleAxisf(m_rotZ, Eigen::Vector3f::UnitZ())
        * Eigen::AngleAxisf(m_rotY, Eigen::Vector3f::UnitY())
        * Eigen::AngleAxisf(m_rotX, Eigen::Vector3f::UnitZ());
    Eigen::Transform3f t(m);
    t *= Eigen::Translation3f(m_position);
    return t * p;
}

Point3 Subdivision::worldToLocalCoords(const Point3& p) const
{
    Eigen::Matrix3f m;
    m = Eigen::AngleAxisf(m_rotZ, Eigen::Vector3f::UnitZ())
        * Eigen::AngleAxisf(m_rotY, Eigen::Vector3f::UnitY())
        * Eigen::AngleAxisf(m_rotX, Eigen::Vector3f::UnitZ());
    Eigen::Transform3f t(m);
    t *= Eigen::Translation3f(m_position);
    Eigen::Vector4f p4(p.x(), p.y(), p.z(), 1.0f);
    return Point3(Eigen::Vector4f(t.inverse() * p4).data());
}

// Inner classes implementation
// Subdivision::VertexIterator
Subdivision::VertexIterator::VertexIterator(const Subdivision* surface, int level)
    :	_surface(surface),
    _level(level)
{
    _index = _surface->_vertices->begin();
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
    int n = _surface->getNumVertices();
//    int n = _level == -1 ? _surface->getNumVertices()
//        : _surface->_vertLevelCollections[_level].size();
//    return n > 0 && _index >= -1 && _index < n-1;
    return n >0 && _index != _surface->_vertices->end();
}

bool Subdivision::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    int n = _surface->getNumVertices();
//    int n = _level == -1 ? _surface->getNumVertices()
//        : _surface->_vertLevelCollections[_level].size();
//    return _index > 0 && n > 0 && _index <= n;
    return n > 0 &&
            (_index == _surface->_vertices->end() ||
             _index != _surface->_vertices->begin());
}

Vertex & Subdivision::VertexIterator::next()
{
    //NOT_IMPLEMENTED
    //return *_surface->_vertices->at(++_index);
    Vertex *v = _index.value();
    ++_index;
    return *v;
}

const Vertex & Subdivision::VertexIterator::next() const
{
    //NOT_IMPLEMENTED
//    return *_surface->_vertices->at(++_index);
    Vertex *v = _index.value();
    ++_index;
    return *v;
}

Vertex & Subdivision::VertexIterator::previous()
{
    //NOT_IMPLEMENTED
//    return *_surface->_vertices->at(_index--);
    --_index;
    return *_index.value();
}

const Vertex & Subdivision::VertexIterator::previous() const
{
    //NOT_IMPLEMENTED
//    return *_surface->_vertices->at(_index--);
    --_index;
    return *_index.value();
}

bool Subdivision::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
//    int n = _surface->getNumVertices();
//    int nindex = 0;
//    switch(origin)
//    {
//    case Iter_Current:
//        nindex = _index + pos;

//        break;
//    case Iter_End:
//        nindex = n + pos - 1;
//        break;

//    case Iter_Start:
//    default:
//        nindex = pos - 1;
//        break;
//    }

//    // check the new pos is valid
//    if (nindex >= n || nindex < 0)
//        return false;

//    // go to the new pos
//    _index = nindex;

    switch(origin)
    {
    case Iter_Current:
        // nothing
        break;
    case Iter_End:
        _index = _surface->_vertices->end();
        break;

    case Iter_Start:
    default:
        _index = _surface->_vertices->begin();
        break;
    }

    if (pos > 0) {
        while(--pos && _index != _surface->_vertices->end()) {
            ++_index;
        }
        if (_index == _surface->_vertices->end())
            return false;
    } else if (pos < 0 ) {
        while(++pos && _index != _surface->_vertices->end()) {
            --_index;
        }
        if (_index == _surface->_vertices->end())
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
    _index = _surface->_faces->begin();
}

IIterator<Face>* Subdivision::FaceIterator::clone() const
{
    FaceIterator *it = new FaceIterator(_surface, _level);
    it->_index = this->_index;
    return it;
}

bool Subdivision::FaceIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    int n = _surface->getNumFaces();
//    return n > 0 && _index < n-1;
    return n > 0 && _index != _surface->_faces->end();
}

bool Subdivision::FaceIterator::hasPrevious() const
{
    int n = _surface->getNumFaces();
//    return n > 0 && _index < n && _index >= 0;
    return n > 0 &&
            (_index == _surface->_faces->end() ||
             _index != _surface->_faces->begin());
}

Face & Subdivision::FaceIterator::next()
{
    //NOT_IMPLEMENTED
//    return *_surface->_faces->at(++_index);
    Face *f = _index.value();

    assert(f);
    ++_index;
    return *f;
}

const Face & Subdivision::FaceIterator::next() const
{
    //NOT_IMPLEMENTED
//    assert(_surface->getNumFaces() > _index);
//    return *_surface->_faces->at(++_index);
    Face *f = _index.value();
    assert(f);
    ++_index;
    return *f;
}

Face & Subdivision::FaceIterator::previous()
{
//    assert(_index >= 0);
//    Face* f = _surface->_faces->at(_index);
//    --_index;
//    return *f;
    --_index;
    return *_index.value();
}

const Face & Subdivision::FaceIterator::previous() const
{
//    assert(_index >= 0);
//    Face* f = _surface->_faces->at(_index);
//    --_index;
//    return *f;
    --_index;
    return *_index.value();
}

bool Subdivision::FaceIterator::seek(int pos, IteratorOrigin origin) const
{
//    int n = _surface->getNumFaces();
//    int nindex = 0;
//    switch(origin)
//    {
//    case Iter_Current:
//        nindex = _index + pos;

//        break;
//    case Iter_End:
//        nindex = n + pos - 1;
//        break;

//    case Iter_Start:
//    default:
//        nindex = pos - 1;
//        break;
//    }

//    // check the new pos is valid
//    if (nindex >= n || nindex < -1)
//        return false;

//    // go to the new pos
//    _index = nindex;

//    return true;

    switch(origin)
    {
    case Iter_Current:
        // nothing
        break;
    case Iter_End:
        _index = _surface->_faces->end();
        break;

    case Iter_Start:
    default:
        _index = _surface->_faces->begin();
        break;
    }

    if (pos > 0) {
        while(--pos && _index != _surface->_faces->end()) {
            ++_index;
        }
        if (_index == _surface->_faces->end())
            return false;
    } else if (pos < 0 ) {
        while(++pos && _index != _surface->_faces->end()) {
            --_index;
        }
        if (_index == _surface->_faces->end())
            return false;
    }
    return true;
}

void Subdivision::printMemoryInfo() const
{
    unsigned int sizeVertex, sizeEdge, sizeFace;
    unsigned int nVertex, nFace, nEdge;
    
    sizeVertex = sizeof(Vertex);
    sizeFace = sizeof(Face);
    sizeEdge = sizeof(Edge);
    nVertex = this->getNumVertices();
    nFace = this->getNumFaces();
    nEdge = this->_edges->size();
    
    qDebug() << "Size of vertex: " << sizeVertex;
    qDebug() << "Size of Face: " << sizeFace;
    qDebug() << "Size of Edge: " << sizeEdge;
    qDebug() << "Num verts: " << nVertex;
    qDebug() << "Num facess: " << nFace;
    qDebug() << "Num edges: " << nEdge;
    
    qDebug() << "Size of vertex hash: " << sizeof(*_vertices);
    
    qDebug() << "Mem usage: " << (sizeVertex * nVertex) 
        + (sizeEdge * nEdge) 
        + (sizeFace * nFace)
        + sizeof(*_vertices);
}
