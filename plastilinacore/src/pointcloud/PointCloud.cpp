//
//  PointCloud.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/pointcloud/PointCloud.h>
#include <PlastilinaCore/pointcloud/PointCloudRenderable.h>
#include <PlastilinaCore/Vertex.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/Face.h>

using core::subdivision::Vertex;
using core::subdivision::Face;

// Iterator classes declarations
class PointCloud::VertexIterator : public IIterator<VertexHandle>
{
    friend class PointCloud;
public:
    typedef std::vector<Vertex*>::iterator iterator;
    
    const PointCloud*  _surface;
    mutable iterator    _index;
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(const PointCloud* surface);
    
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

class PointCloud::FaceIterator : public IIterator<FaceHandle>
{
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator() {};
    
public:
    /**
     *
     */
    IIterator<FaceHandle>* clone() const {
        return nullptr;
    };
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const {
        return false;
    }
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    bool hasPrevious() const {
        return false;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr next() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const shared_ptr previous() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const  {
        return false;
    }
};

class PointCloud::EdgeIterator : public IIterator<EdgeHandle>
{
    
protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator() {};
    
public:
    /**
     *
     */
    IIterator<EdgeHandle>* clone() const {
        return nullptr;
    };
    
    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const {
        return false;
    }
    
    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    bool hasPrevious() const {
        return false;
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr next() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    const shared_ptr next() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    shared_ptr peekNext() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the next element without advancing to the next
     */
    const shared_ptr peekNext() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    shared_ptr previous() {
        return shared_ptr(nullptr);
    }
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    const shared_ptr previous() const {
        return shared_ptr(nullptr);
    }
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    bool seek(int pos, IteratorOrigin origin) const  {
        return false;
    }
};


struct PointCloud::Impl {
    /** Instance ID of the surface */
	ISurface::size_t _iid;
    
	geometry::AABB			boundingBox;
    Color					color;
    bool					isSelected;
    std::vector<Vertex*>  	vertices;
    Scene*          		scene;
    bool            		hasChanged;
    std::vector<VertexHandle::size_t>    selectedPoints;
    
	PointCloudRenderable renderable;
};

PointCloud::PointCloud()
: d(new Impl)
{
}

PointCloud::~PointCloud(){
}

ISurface::size_t PointCloud::iid() const
{
    return d->_iid;
}


void PointCloud::setScene(Scene* scene)
{
    d->scene = scene;
}

Scene* PointCloud::scene() const
{
    return d->scene;
}

const IRenderable* PointCloud::renderable() const
{
    return &d->renderable;
}

const geometry::AABB& PointCloud::boundingBox() const
{
    return d->boundingBox;
}

void PointCloud::setColor(const Color& c)
{
    d->color = c;
}

Color PointCloud::color() const
{
    return d->color;
}

void PointCloud::setSelected(bool val)
{
    d->isSelected = val;
}

bool PointCloud::isSelected() const
{
    return d->isSelected;
}

ISurface::size_t PointCloud::addVertex(const Point3& point)
{
    Vertex * v = new Vertex(point);
    return addVertex(v);
}

ISurface::size_t PointCloud::addVertex(VertexHandle* v)
{
    Vertex * vtx = static_cast<Vertex*>(v);
    d->vertices.push_back(vtx);
    return v->iid();
}

void PointCloud::removeVertex( ISurface::size_t id)
{
    std::remove_if(d->vertices.begin(), d->vertices.end(),
                   [id](const Vertex* v) {
                        return v->iid() == id;
                    }
                   );
}

VertexHandle* PointCloud::vertex(ISurface::size_t iid)
{
    auto it = std::find_if(d->vertices.begin(), d->vertices.end(),
                           [iid](const Vertex* v) {
                               return v->iid() == iid;
                           }
                           );
    return it != d->vertices.end() ? *it : nullptr;
}

const VertexHandle* PointCloud::vertex(ISurface::size_t iid) const
{
    auto it = std::find_if(d->vertices.begin(), d->vertices.end(),
                           [iid](const Vertex* v) {
                               return v->iid() == iid;
                           }
                           );
    return it != d->vertices.end() ? *it : nullptr;
}

ISurface::size_t PointCloud::numVertices() const
{
    std::vector<VertexHandle*>::size_type s = d->vertices.size();
    assert(s <= std::numeric_limits<ISurface::size_t>::max());
    return static_cast<ISurface::size_t>(s);
}

ISurface::size_t PointCloud::addEdge(const EdgeHandle& edge)
{
    return 0;
}

ISurface::size_t PointCloud::addEdge(ISurface::size_t v1,
                                     ISurface::size_t v2)
{
    return 0;
}

ISurface::size_t PointCloud::numEdges() const
{
    return 0;
}

ISurface::size_t PointCloud::addFace( const std::vector<ISurface::size_t>& vertexIndexes )
{
    return 0;
}

void PointCloud::replaceFace(ISurface::size_t index,
                             const std::vector<ISurface::size_t>& vertexIndexList)
{
    
}

void PointCloud::removeFace( ISurface::size_t id)
{
    
}

ISurface::size_t PointCloud::numFaces() const
{
    return 0;
}

FaceHandle* PointCloud::face(ISurface::size_t index)
{
    return nullptr;
}

ISurface::size_t PointCloud::getFaceIndexAtPoint(const Point3& p) const
{
    return 0;
}

ISurface::size_t PointCloud::getClosestPointAtPoint(const Point3 &p) const
{
    NOT_IMPLEMENTED
}

std::vector<ISurface::size_t> PointCloud::getPointsInRadius(const Point3 &p,
                                                            float radius) const
{
    NOT_IMPLEMENTED
}

void PointCloud::adjustPointNormal(ISurface::size_t index)
{
    NOT_IMPLEMENTED
}

void PointCloud::lock() const
{
    NOT_IMPLEMENTED
}

void PointCloud::unlock() const
{
    NOT_IMPLEMENTED
}

bool PointCloud::hasChanged()
{
    return d->hasChanged;
}

void PointCloud::setChanged(bool val)
{
    d->hasChanged = val;
}

std::vector<ISurface::size_t> PointCloud::selectedPoints() const
{
    NOT_IMPLEMENTED
}

void PointCloud::setSelectedPoints(const std::vector<ISurface::size_t>& indicesArray)
{
    NOT_IMPLEMENTED
}

Iterator<VertexHandle> PointCloud::vertexIterator()
{
    return Iterator<VertexHandle>(new VertexIterator(this));
}

Iterator<VertexHandle> PointCloud::constVertexIterator() const
{
    return Iterator<VertexHandle>(new VertexIterator(this));
}

Iterator<FaceHandle> PointCloud::faceIterator()
{
    NOT_IMPLEMENTED
}

Iterator<FaceHandle> PointCloud::constFaceIterator() const
{
    NOT_IMPLEMENTED
}

Iterator<EdgeHandle> PointCloud::edgeIterator()
{
    NOT_IMPLEMENTED
}

Iterator<EdgeHandle> PointCloud::constEdgeIterator() const
{
    NOT_IMPLEMENTED
}

void PointCloud::meshChanged(ISurface* mesh)
{
    NOT_IMPLEMENTED
}


//////////////////////////////////////////////////////////////////////////////
// Inner classes implementation
// PointCloud::VertexIterator
PointCloud::VertexIterator::VertexIterator(const PointCloud* surface)
:	_surface(surface)
{
    _index = _surface->d->vertices.begin();
}


IIterator<VertexHandle>* PointCloud::VertexIterator::clone() const
{
    VertexIterator *it = new VertexIterator(_surface);
    it->_index = this->_index;
    return it;
}

bool PointCloud::VertexIterator::hasNext() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n >0 && _index != _surface->d->vertices.end();
}

bool PointCloud::VertexIterator::hasPrevious() const
{
    //NOT_IMPLEMENTED
    size_t n = _surface->numVertices();
    return n > 0 &&
    (_index == _surface->d->vertices.end() ||
     _index != _surface->d->vertices.begin());
}

PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::next()
{
    Vertex *v = *_index;
    ++_index;
    return v;
}

const PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::next() const
{
    Vertex *v = *_index;
    ++_index;
    return v;
}

PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::peekNext() {
    return *_index;
}

const PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::peekNext() const {
    return *_index;
}

PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::previous()
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

const PointCloud::VertexIterator::shared_ptr PointCloud::VertexIterator::previous() const
{
    NOT_IMPLEMENTED
    //    --_index;
    //    return _index->second;
}

bool PointCloud::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    switch(origin)
    {
        case Iter_Current:
            // nothing
            break;
        case Iter_End:
            _index = _surface->d->vertices.end();
            break;
            
        case Iter_Start:
        default:
            _index = _surface->d->vertices.begin();
            break;
    }
    
    if (pos > 0) {
        while(--pos && _index != _surface->d->vertices.end()) {
            ++_index;
        }
        if (_index == _surface->d->vertices.end())
            return false;
    } else if (pos < 0 ) {
        std::cerr << "PointCloud::VertexIterator::seek: not supported" << std::endl;
        return false;
        //        while(++pos && _index != _surface->_d->_vertices->end()) {
        //            --_index;
        //        }
        //        if (_index == _surface->_d->_vertices->end())
        //            return false;
    }
    return true;
}

