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

#include "IIterator.h"
#include "Vertex.h"

class Face;

enum EdgeFlags {
    EF_None     = 0,
    EF_Selected = 1 << 0,   /*< Face is selected */
    EF_Deleted  = 1 << 31,  /*< Face is marked as deleted */
    EF_All      = 0xFFFFFFFF
};

class Edge {
    static QAtomicInt NEXT_ID;

    int _id;
    EdgeFlags   _flags;
    Edge *_next, *_pair;
    Vertex *_head, *_tail;
    Face *_face;

public:
    Edge();

    Edge(Vertex* tail, Vertex* head);

    ~Edge();

    /**
     * Gets the instance id of the vertex.
     */
    int iid() const { return _id; }

    /**
     * Set / get attribute flags to the face.
     */
    void addFlag(EdgeFlags flag) { _flags = (EdgeFlags)(_flags | flag); }
    void removeFlag(EdgeFlags flag) { _flags = (EdgeFlags)(_flags & ~flag); }
    EdgeFlags flags() const { return _flags; }
    void setFlags(EdgeFlags flags) {_flags = flags; }

    /**
     * Gets / sets a pointer to the next HEdge structure.
     */
    Edge* next() const { return _next; }
    void setNext(Edge* he) { _next = he; }

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
    Vertex* tail() const { return _tail; }
    void setTail(Vertex* v) { _tail = v; }

    /**
     * Gets / sets a pointer to a face this half edge belongs to.
     */
    Face* face() const { return _face; }
    void setFace(Face* f) { _face = f; }

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
    friend uint qHash(const Edge&);
};


struct EdgePtrComparator
{
    const Edge* _e;

    EdgePtrComparator(const Edge& e) : _e(&e) {}

    bool operator()(const Edge* e) {
        return (*_e) == (*e);
    }
};

struct EdgePtrComparator2
{
    const Vertex *_tail;
    const Vertex *_head;

    EdgePtrComparator2(const Vertex *tail, const Vertex *head) : _tail(tail), _head(head) {}

    bool operator()(const Edge* e) {
        return (e->head()->iid() == _head->iid() &&
                e->tail()->iid() == _tail->iid());
        //return (*_e) == (*e);
    }
};


inline uint qHash(const Edge& key)
{
    if (key._head->iid() > key._tail->iid())
        return qHash( ((quint64)key._head->iid()) << 32 |
                      (quint64)key._tail->iid() );
    else
        return qHash( ((quint64)key._tail->iid()) << 32 |
                      (quint64)key._head->iid() );
}

#endif
