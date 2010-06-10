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
    
    const Subdivision*  _surface;
    int                 _level;
    mutable int         _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(const Subdivision* surface, int level = -1);
    
public:
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
};

class Subdivision::FaceIterator : public IIterator<Face>
{
    friend class Subdivision;
    
    const Subdivision*  _surface;
    int                 _level;
    mutable int         _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(const Subdivision* v, int level = -1);
    
public:
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
};



Subdivision::Subdivision()
:	ISurface(),
	m_scene(NULL),
	//m_drawingMode(Wireframe),
	m_color(Qt::white),
	m_boundingBoxColor(Qt::red),
	m_minX(0.0),
	m_maxX(0.0),
	m_minY(0.0),
	m_maxY(0.0),
	m_minZ(0.0),
	m_maxZ(0.0),
	m_rotX(0.0),
	m_rotY(0.0),
	m_rotZ(0.0),
	m_selected(false),
	m_callListId(0),
	m_genereateCallList(true),
	m_currentResolutionLevel(0),
	m_hasChanged(true),
	_vertices(NULL),
	_edges(NULL),
	_faces(NULL)
{
    initPoints();
    //updateBoundingBox();
}

Subdivision::~Subdivision()
{
	_vertices = NULL;
	_edges = NULL;
	_faces = NULL;
	
	std::vector<VertexCollection>::iterator it = _vertLevelCollections.begin();
	for (; it != _vertLevelCollections.end(); ++it)
	{
		_vertices = &(*it);
		//Vertex
	}
}

void Subdivision::initPoints()
{
	_vertices = new VertexCollection;
	_edges = new EdgesCollection;
	_faces = new FacesCollection;
	
	_vertLevelCollections.push_back(*_vertices);
	_edgesLevelCollections.push_back(*_edges);
	_facesLevelCollections.push_back(*_faces);
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

float Subdivision::getDepth()
{
    return m_maxZ - m_minZ;
}

float Subdivision::getHeight()
{
    return m_maxY - m_minY;
}

float Subdivision::getWidth()
{
    return m_maxX - m_minX;
}

void Subdivision::setDepth(float value)
{
    //scale(1.0, 1.0, value / m_depth);
    //m_depth = value;
    //updateBoundingBox();
    Q_UNUSED(value);
}

void Subdivision::setHeight(float value)
{
    //scale(1.0, value / m_height, 1.0);
    //m_height = value;
    //updateBoundingBox();
    Q_UNUSED(value);
}

void Subdivision::setWidth(float value)
{
    //scale(value / m_width, 1.0, 1.0);
    //m_width = value;
    //updateBoundingBox();
    Q_UNUSED(value);
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

void Subdivision::setBoundingBoxColor(const QColor& color)
{
    m_boundingBoxColor = color;
}

QColor Subdivision::getBoundingBoxColor() const
{
    return m_boundingBoxColor;
}

int Subdivision::addVertex(const Point3& point)
{
	assert(_vertices != NULL );
	//qWarning("%s %s", __FUNCTION__, " Not implemented");
    float x = point[0];
    float y = point[1];
    float z = point[2];
	
    m_minX = MIN(x , m_minX);
    m_minY = MIN(y , m_minY);
    m_minZ = MIN(z , m_minZ);
	
    m_maxX = MAX(x , m_maxX);
    m_maxY = MAX(y , m_maxY);
    m_maxZ = MAX(z , m_maxZ);
	
	Vertex* vertex = new Vertex(point);
	this->_vertices->push_back(vertex);
    return _vertices->size() - 1;
}

int Subdivision::addVertex(Vertex* v)
{
    float x = v->getPosition().x();
    float y = v->getPosition().y();
    float z = v->getPosition().z();
	
    m_minX = MIN(x , m_minX);
    m_minY = MIN(y , m_minY);
    m_minZ = MIN(z , m_minZ);
	
    m_maxX = MAX(x , m_maxX);
    m_maxY = MAX(y , m_maxY);
    m_maxZ = MAX(z , m_maxZ);
	
    _vertices->push_back(v);
    return _vertices->size() - 1;
}

void Subdivision::removeVertex(int id)
{
	qWarning("%s %s", __FUNCTION__, " Not implemented");
}

Point3& Subdivision::getVertex(int index)
{
	//qWarning("%s %s", __FUNCTION__, " Not implemented");
    return _vertices->at(index)->position();
}

Point3 Subdivision::getVertex(int index) const
{
    return _vertices->at(index)->position();
}

int Subdivision::getNumVertices() const
{
	return _vertices->size();
}

int Subdivision::addEdge(const Edge& edge)
{
	//NOT_IMPLEMENTED
	int indexOf = -1; 
    
    std::vector<Edge*>::iterator it;
    it = std::find_if(_edges->begin(), _edges->end(), EdgePtrComparator(edge));
    if (it != _edges->end()) {
        indexOf = it - _edges->begin();
    } else {
        Edge *h1 = new Edge(edge.tail(), edge.head());
        Edge *h2 = new Edge(edge.head(), edge.tail());
        h1->setPair(h2);
        h2->setPair(h1);
        
        h1->head()->setEdge(h2);
        
        indexOf = _edges->size();
        _edges->push_back(h1);
        _edges->push_back(h2);
    }
    
	return indexOf;
}

int Subdivision::addEdge(int v1, int v2)
{
	Edge edge(_vertices->at(v1), _vertices->at(v2));
    
	return addEdge(edge);
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
        int index = addEdge(vertexIndexList[i], vertexIndexList[(i+1) % size]);
        assert(index != -1);
        edges.push_back(_edges->at(index));
    }
    Face *f = new Face(this);
    f->setHEdge(edges[0]);
    for (int i = 0; i < size; ++i) {
        edges[i]->setNext(edges[(i+1)%size]);
        edges[i]->setFace(f);
        //_edges[edgesIndices[i]].setNext(edgesIndices[(i+1)%size]);
    }
    _faces->push_back(f);
    return _faces->size() - 1;
}

void Subdivision::replaceFace(int faceIndex, const QVector<int>& vertexIndexList)
{
    NOT_IMPLEMENTED
}

void Subdivision::removeFace( int id)
{
	NOT_IMPLEMENTED
}

Face& Subdivision::getFace(int index)
{
    assert(index < _faces->size());
    return *_faces->at(index);
}

int Subdivision::getNumFaces() const
{
	assert( _faces!= NULL );
	return _faces->size();
}

void Subdivision::drawBoundingBox()
{
    GLboolean lightEnabled = GL_FALSE;
    lightEnabled = glIsEnabled(GL_LIGHTING);
	
    if (lightEnabled == GL_TRUE)
        glDisable(GL_LIGHTING);
	
    glColor3f(0.0, 1.0, 0.0);
	
    float minX = m_minX - 0.1;
    float maxX = m_maxX + 0.1;
    float minY = m_minY - 0.1;
    float maxY = m_maxY + 0.1;
    float minZ = m_minZ - 0.1;
    float maxZ = m_maxZ + 0.1;
	
    glColor3f(0.0, 1.0, 0.0);
	
    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, minZ);
    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, maxY, minZ);
    glVertex3f(minX, maxY, minZ);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(minX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();
	
    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ);
    glVertex3f(minX, minY, maxZ);
	
    glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, minY, maxZ);
	
    glVertex3f(maxX, maxY, minZ);
    glVertex3f(maxX, maxY, maxZ);
	
    glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, maxY, maxZ);
    glEnd();
	
    if (lightEnabled == GL_TRUE)
        glEnable(GL_LIGHTING);
}

int Subdivision::getFaceIndexAtPoint(const Point3& p) const
{
    NOT_IMPLEMENTED
	
    return -1;
}

int Subdivision::getClosestPointAtPoint(const Point3 &p) const
{
    NOT_IMPLEMENTED
	
    return -1;
}

QVector<int> Subdivision::getPointsInRadius(const Point3 &p, float radius) const
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

void Subdivision::adjustPointNormal(int index)
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

// Inner classes implementation
// Subdivision::VertexIterator
Subdivision::VertexIterator::VertexIterator(const Subdivision* surface, int level)
:	_surface(surface),
    _level(level),
	_index(-1)
{
}

bool Subdivision::VertexIterator::hasNext() const
{
	//NOT_IMPLEMENTED
	int n = _level == -1 ? _surface->getNumVertices() 
            : _surface->_vertLevelCollections[_level].size();
	return n > 0 && _index < n-1;
}

bool Subdivision::VertexIterator::hasPrevious() const
{
	//NOT_IMPLEMENTED
	//int n = _surface->getNumVertices();
    int n = _level == -1 ? _surface->getNumVertices() 
            : _surface->_vertLevelCollections[_level].size();
	return _index > 0 && n > 0 && _index <= n;
}

Vertex & Subdivision::VertexIterator::next()
{
	//NOT_IMPLEMENTED
	return *_surface->_vertices->at(++_index);
}

const Vertex & Subdivision::VertexIterator::next() const
{
	//NOT_IMPLEMENTED
	return *_surface->_vertices->at(++_index);
}

Vertex & Subdivision::VertexIterator::previous()
{
	//NOT_IMPLEMENTED
	return *_surface->_vertices->at(_index--);
}

const Vertex & Subdivision::VertexIterator::previous() const
{
	//NOT_IMPLEMENTED
	return *_surface->_vertices->at(_index--);
}


//Subdivision::FaceIterator
Subdivision::FaceIterator::FaceIterator(const Subdivision* surface, int level)
:	_surface(surface),
    _level(level),
	_index(-1)
{
}

bool Subdivision::FaceIterator::hasNext() const
{
	//NOT_IMPLEMENTED
	int n = _surface->getNumFaces();
	return n > 0 && _index < n-1;
}

bool Subdivision::FaceIterator::hasPrevious() const
{
	NOT_IMPLEMENTED
	return false;
}

Face & Subdivision::FaceIterator::next()
{
	//NOT_IMPLEMENTED
	return *_surface->_faces->at(++_index);
}

const Face & Subdivision::FaceIterator::next() const
{
	//NOT_IMPLEMENTED
	return *_surface->_faces->at(++_index);
}

Face & Subdivision::FaceIterator::previous()
{
	NOT_IMPLEMENTED
	static Face f;
	return f;
}

const Face & Subdivision::FaceIterator::previous() const
{
	NOT_IMPLEMENTED
	static Face f;
	return f;
}


