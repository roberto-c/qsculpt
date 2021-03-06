/*
 *  Vertex.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 1/6/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Face.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/Vertex.h>
#include <sstream>

namespace core
{
namespace subdivision
{

class Vertex::VertexIterator : public IIterator<Vertex>
{
  public:
    typedef Vertex::shared_ptr shared_ptr;
    typedef Vertex::weak_ptr   weak_ptr;
    typedef Vertex::ptr        ptr;

  private:
    friend class Vertex;

    // Vertex::shared_ptr         _v;
    mutable Edge::shared_ptr _iniHe;
    mutable Edge::shared_ptr _nextHe;
    mutable bool             _firstIt;

  protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    VertexIterator(Vertex* v);

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
    const_shared_ptr next() const;

    /**
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr peekNext();

    /**
     * Returns the next element and advance the iterator by one.
     */
    const_shared_ptr peekNext() const;

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
    bool seek(int pos, IteratorOrigin origin) const;
};

class Vertex::FaceIterator : public IIterator<Face>
{
  public:
    typedef Face::shared_ptr shared_ptr;
    typedef Face::weak_ptr   weak_ptr;
    typedef Face::ptr        ptr;

  private:
    friend class Vertex;

    // Vertex::shared_ptr       _v;
    mutable Edge::shared_ptr _iniHe;
    mutable Edge::shared_ptr _nextHe;
    mutable bool             _firstIt;

  protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    FaceIterator(Vertex* v);

  public:
    /**
     *
     */
    IIterator<Face>* clone() const;

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
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr peekNext();

    /**
     * Returns the next element and advance the iterator by one.
     */
    const_shared_ptr peekNext() const;

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
    bool seek(int pos, IteratorOrigin origin) const;
};

class Vertex::EdgeIterator : public IIterator<Edge>
{
  public:
    typedef Edge::shared_ptr shared_ptr;
    typedef Edge::weak_ptr   weak_ptr;
    typedef Edge::ptr        ptr;

  private:
    friend class Vertex;

    // Vertex::shared_ptr           _v;
    mutable Edge::shared_ptr _iniHe;
    mutable Edge::shared_ptr _nextHe;
    mutable bool             _firstIt;

  protected:
    /**
     * Constructor of a vertex iterator. The vertex iterator
     * is only contructed by means of Vertex::vertexIterator() function
     */
    EdgeIterator(Vertex* v);

  public:
    /**
     *
     */
    IIterator<Edge>* clone() const;

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
     * Returns the next element and advance the iterator by one.
     */
    shared_ptr peekNext();

    /**
     * Returns the next element and advance the iterator by one.
     */
    const_shared_ptr peekNext() const;

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
    bool seek(int pos, IteratorOrigin origin) const;
};

std::atomic_int Vertex::NEXT_ID(1);

Vertex::Vertex()
    : _position(Point3(0, 0, 0))
    , _normal(Vector3(0, 1, 0))
    , _color(Color(1, 1, 1, 1))
    , _flags(VF_None)
    , _he(NULL)
    , _userData(NULL)
{
    _id = NEXT_ID.fetch_add(1);
}

Vertex::Vertex(const Point3& position, const Vector3& normal,
               const Color& color, const Point2& texCoords)
    : _position(position)
    , _normal(normal.normalized())
    , _color(color)
    , _texCoords(texCoords)
    , _flags(VF_None)
    , _he(NULL)
    , _userData(NULL)
{
    _id = NEXT_ID.fetch_add(1);
}

Vertex::Vertex(const Vertex& v)
{
    _id       = NEXT_ID.fetch_add(1);
    _position = v._position;
    _normal   = v._normal;
    _color    = v._color;
    _flags    = v._flags;
    _he       = v._he;
    _userData = v._userData;
}

Vertex& Vertex::operator=(const Vertex& v)
{
    if (this == &v)
        return *this;

    _position = v._position;
    _normal   = v._normal;
    _color    = v._color;
    _he       = v._he;

    return *this;
}

bool Vertex::operator==(const Vertex& v) const
{
    if (this == &v)
        return true;

    return (_position == v._position && _normal == v._normal &&
            _color.data() == v._color.data());
}

bool Vertex::operator!=(const Vertex& v) const { return !operator==(v); }

Iterator<Vertex> Vertex::vertexIterator()
{
    return Iterator<Vertex>(new VertexIterator(this));
}

Iterator<Face> Vertex::faceIterator()
{
    return Iterator<Face>(new FaceIterator(this));
}

Iterator<Edge> Vertex::edgeIterator()
{
    return Iterator<Edge>(new EdgeIterator(this));
}

std::string Vertex::toString()
{
    std::ostringstream str;
    str << "V_" << _id << ":(" << _position.x() << ",";
    str << _position.y() << "," << _position.z() << ")";

    return str.str();
}

// VertexIterator

Vertex::VertexIterator::VertexIterator(Vertex* v)
    : _iniHe(v ? v->edge() : NULL)
    , _nextHe(NULL)
    , _firstIt(true)
{
    _nextHe  = _iniHe; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL;
}

IIterator<Vertex>* Vertex::VertexIterator::clone() const
{
    VertexIterator* it = new VertexIterator(NULL);
    it->_nextHe        = _nextHe;
    it->_iniHe         = _iniHe;
    it->_firstIt       = _firstIt;
    return it;
}

bool Vertex::VertexIterator::hasNext() const
{
    return (_firstIt) || ((_nextHe != NULL) && (_iniHe != _nextHe));
}

bool Vertex::VertexIterator::hasPrevious() const
{
    NOT_IMPLEMENTED
    return false;
}

Vertex::VertexIterator::shared_ptr Vertex::VertexIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt  = false;
    Vertex* v = _nextHe->head();
    _nextHe   = _nextHe->pair()->next();
    return v;
}

Vertex::VertexIterator::const_shared_ptr Vertex::VertexIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt  = false;
    Vertex* v = _nextHe->head();
    _nextHe   = _nextHe->pair()->next();
    return v;
}

Vertex::VertexIterator::shared_ptr Vertex::VertexIterator::peekNext()
{
    return _nextHe->head();
}

Vertex::VertexIterator::const_shared_ptr
Vertex::VertexIterator::peekNext() const
{
    return _nextHe->head();
}

Vertex::VertexIterator::shared_ptr Vertex::VertexIterator::previous(){
    NOT_IMPLEMENTED}

Vertex::VertexIterator::const_shared_ptr Vertex::VertexIterator::previous()
    const
{
    NOT_IMPLEMENTED
}

bool Vertex::VertexIterator::seek(int pos, IteratorOrigin origin) const
{
    return false;
}

// FaceIterator

Vertex::FaceIterator::FaceIterator(Vertex* v)
    : _iniHe(v ? v->edge() : NULL)
    , _nextHe(NULL)
    , _firstIt(true)
{
    _nextHe  = _iniHe; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL && _nextHe->face() != NULL;
}

IIterator<Face>* Vertex::FaceIterator::clone() const
{
    FaceIterator* it = new FaceIterator(NULL);
    it->_nextHe      = _nextHe;
    it->_iniHe       = _iniHe;
    it->_firstIt     = _firstIt;
    return it;
}

bool Vertex::FaceIterator::hasNext() const
{
    return (_firstIt) || ((_nextHe != NULL) && (_iniHe != _nextHe) &&
                          (_nextHe->face() != NULL));
}

bool Vertex::FaceIterator::hasPrevious() const {NOT_IMPLEMENTED}

Vertex::FaceIterator::shared_ptr Vertex::FaceIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Face* f  = _nextHe->face();
    _nextHe  = _nextHe->pair()->next();
    return f;
}

Vertex::FaceIterator::const_shared_ptr Vertex::FaceIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Face* f  = _nextHe->face();
    _nextHe  = _nextHe->pair()->next();
    return f;
}

Vertex::FaceIterator::shared_ptr Vertex::FaceIterator::peekNext()
{
    return _nextHe->face();
}

Vertex::FaceIterator::const_shared_ptr Vertex::FaceIterator::peekNext() const
{
    return _nextHe->face();
}

Vertex::FaceIterator::shared_ptr Vertex::FaceIterator::previous(){
    NOT_IMPLEMENTED}

Vertex::FaceIterator::const_shared_ptr Vertex::FaceIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Vertex::FaceIterator::seek(int pos, IteratorOrigin origin) const
{
    NOT_IMPLEMENTED
    return false;
}

// EdgeIterator

Vertex::EdgeIterator::EdgeIterator(Vertex* v)
    : _iniHe(v ? v->edge() : NULL)
    , _nextHe(NULL)
    , _firstIt(true)
{
    _nextHe  = _iniHe; //? _iniHe->pair()->next() : NULL;
    _firstIt = _nextHe != NULL && _nextHe->face() != NULL;
}

IIterator<Edge>* Vertex::EdgeIterator::clone() const
{
    EdgeIterator* it = new EdgeIterator(NULL);
    it->_nextHe      = _nextHe;
    it->_iniHe       = _iniHe;
    it->_firstIt     = _firstIt;
    return it;
}

bool Vertex::EdgeIterator::hasNext() const
{
    return (_firstIt) || ((_nextHe != NULL) && (_iniHe != _nextHe) &&
                          (_nextHe->face() != NULL));
}

bool Vertex::EdgeIterator::hasPrevious() const {NOT_IMPLEMENTED}

Vertex::EdgeIterator::shared_ptr Vertex::EdgeIterator::next()
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Edge* e  = _nextHe;
    _nextHe  = _nextHe->pair()->next();
    return e;
}

Vertex::EdgeIterator::const_shared_ptr Vertex::EdgeIterator::next() const
{
    assert(_nextHe != NULL);
    _firstIt = false;
    Edge* e  = _nextHe;
    _nextHe  = _nextHe->pair()->next();
    return e;
}

Vertex::EdgeIterator::shared_ptr Vertex::EdgeIterator::peekNext()
{
    return _nextHe;
}

Vertex::EdgeIterator::const_shared_ptr Vertex::EdgeIterator::peekNext() const
{
    return _nextHe;
}

Vertex::EdgeIterator::shared_ptr Vertex::EdgeIterator::previous(){
    NOT_IMPLEMENTED}

Vertex::EdgeIterator::const_shared_ptr Vertex::EdgeIterator::previous() const
{
    NOT_IMPLEMENTED
}

bool Vertex::EdgeIterator::seek(int pos, IteratorOrigin origin) const
{
    NOT_IMPLEMENTED
    return false;
}

} // namespace subdivision
} // namespace core

Vector4 VertexHandle::position() const
{
    switch ((VertexHandleType)this->type())
    {
    case VertexHandleType::DEFAULT:
        auto v =
            static_cast<const core::subdivision::Vertex*>(this)->position();
        return Vector4(v.x(), v.y(), v.z(), 1);
        // case VertexHandleType::GPUSUBDIVISION:
        //    return static_cast<const
        //    core::gpusubdivision::Vertex*>(this)->p;
    };
    return Vector4();
}

Vector4 VertexHandle::normal() const
{
    switch ((VertexHandleType)this->type())
    {
    case VertexHandleType::DEFAULT:
        auto v =
            static_cast<const core::subdivision::Vertex*>(this)->normal();
        return Vector4(v.x(), v.y(), v.z(), 0);
        // case VertexHandleType::GPUSUBDIVISION:
        //    return static_cast<const
        //    core::gpusubdivision::Vertex*>(this)->p;
    };
    return Vector4();
}
