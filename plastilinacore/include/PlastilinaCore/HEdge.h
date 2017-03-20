/*
 *  HEdge.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef HEDGE_H_
#define HEDGE_H_

#include <PlastilinaCore/Face.h>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/Vertex.h>
#include <atomic>

enum class EdgeHandleType
{
    NULLTYPE = 0,
    DEFAULT,
    GPUSUBDIVISION,

    MAX = 255
};

class EdgeHandle
{
  public:
    typedef EdgeHandle*       shared_ptr;
    typedef const EdgeHandle* const_shared_ptr;
    typedef EdgeHandle*       weak_ptr;
    typedef EdgeHandle*       ptr;
    typedef const EdgeHandle* const_ptr;
    typedef uint32_t          size_t;

    size_t _id;

    EdgeHandle(EdgeHandleType type = EdgeHandleType::NULLTYPE)
        : _id(0 | ((int)type << 24))
    {
    }

    EdgeHandle(const EdgeHandle&) = default;

    EdgeHandle(EdgeHandle&&) = default;

    uint8_t type() const { return _id >> 24; }
    size_t  iid() const { return _id & 0xFFFFFF; }

    void setIid(size_t iid) { _id = (_id & 0xFF000000) | (iid & 0xFFFFFF); }

    template <typename T>
    const T& cast() const
    {
        return *static_cast<T*>(this);
    }

    template <typename T>
    T& cast()
    {
        return *static_cast<T*>(this);
    }
};

enum EdgeFlags
{
    EF_None     = 0,
    EF_Selected = 0x00000001, /*< Face is selected */
    EF_Deleted  = 0x00000002, /*< Face is marked as deleted */
    EF_Crease   = 0x00000004,
    EF_User1    = 0x00100000, /*< Reserved to use for the user*/
    EF_User2    = 0x00200000, /*< Reserved to use for the user*/
    EF_All      = 0xFFFFFFFF
};

namespace core
{
namespace subdivision
{
class Vertex;
class Face;

class Edge : public EdgeHandle
{
  public:
    typedef Edge*       shared_ptr;
    typedef const Edge* const_shared_ptr;
    typedef Edge*       weak_ptr;
    typedef Edge*       ptr;
    typedef const Edge* const_ptr;

    typedef uint32_t size_t;

  private:
    static std::atomic_int NEXT_ID;

    EdgeFlags        _flags;    // 4
    Edge::weak_ptr   _next;     // 8
    Edge::weak_ptr   _pair;     // 8
    Vertex::weak_ptr _head;     // 8
    Face::weak_ptr   _face;     // 8
    void*            _userData; // 4

  public:
    Edge();

    Edge(Vertex* tail, Vertex* head);

    ~Edge();

    /**
     * Gets the instance id of the vertex.
     */
    size_t iid() const { return _id; }

    /**
     * Set / get attribute flags to the face.
     */
    void addFlag(EdgeFlags flag) { _flags = (EdgeFlags)(_flags | flag); }
    void removeFlag(EdgeFlags flag) { _flags = (EdgeFlags)(_flags & ~flag); }
    EdgeFlags                 flags() const { return _flags; }
    void setFlags(EdgeFlags flags) { _flags = flags; }
    bool isFlagSet(EdgeFlags flag) { return (_flags & flag) != 0; }

    /**
     * Gets / sets a pointer to the next HEdge structure.
     */
    Edge* next() const { return _next; }
    void setNext(Edge* he) { _next = he; }

    //    Edge* prev() const { return _prev; }
    //    void setPrev(Edge* he) { _prev = he; }

    /**
     * Gets / sets a pointer to the pair of this HEdge structure.
     */
    Edge* pair() const { return _pair; }
    void setPair(Edge* he) { _pair = he; }

    /**
     * Gets / sets a pointer to the vertex this structure points to.
     */
    Vertex* head() const { return _head; }
    void setHead(Vertex* v) { _head = v; }

    /**
     * Gets / sets a pointer to the vertex this structure points to.
     */
    //    Vertex* tail() const { return _tail; }
    //    void setTail(Vertex* v) { _tail = v; }

    /**
     * Gets / sets a pointer to a face this half edge belongs to.
     */
    Face* face() const { return _face; }
    void setFace(Face* f) { _face = f; }

    /**
     * Allows to attach a pointer to a user defined structure or data
     */
    void setUserData(void* data) { _userData = data; }

    /**
     * Returns a pointer to the user data.
     *
     * The user is responsible to cast the pointer to the correct type
     */
    void* userData() const { return _userData; }

    bool isEqual(const Edge& he) const;
    bool operator==(const Edge& he) const;
    bool operator!=(const Edge& he) const;

    Iterator<Vertex> vertexIterator();

    Iterator<Edge> hedgeIterator();

  private:
    class VertexIterator;
    class HEdgeIterator;

    friend class VertexIterator;
    friend class HEdgeIterator;
    friend uint32_t qHash(const Edge&);
};

struct EdgePtrComparator
{
    const Edge* _e;

    EdgePtrComparator(const Edge& e)
        : _e(&e)
    {
    }

    bool operator()(const Edge* e) { return (*_e) == (*e); }
};

struct EdgePtrComparator2
{
    const Vertex* _tail;
    const Vertex* _head;

    EdgePtrComparator2(const Vertex* tail, const Vertex* head)
        : _tail(tail)
        , _head(head)
    {
    }

    bool operator()(const Edge* e)
    {
        return (e->head()->iid() == _head->iid() &&
                e->pair()->head()->iid() == _tail->iid());
        //		return (e->head()->iid() == _head->iid() &&
        //                e->tail()->iid() == _tail->iid());
        // return (*_e) == (*e);
    }
};

} // namespace subdivision
} // namespace core

#endif
