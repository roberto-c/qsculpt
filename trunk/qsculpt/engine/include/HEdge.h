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

class Edge {
	static QAtomicInt NEXT_ID;
	
	int _id;
	Edge *_next, *_pair;
	Vertex *_head, *_tail;
	Face *_face;
//	int _next, _pair;
//	int _head;
//    int _tail;
//	int _face;
	
public:
	Edge();
	
	Edge(Vertex* tail, Vertex* head);
	
	~Edge();
	
	/**
	 * Gets / sets a pointer to the next HEdge structure.
	 */
	Edge* next() const { return _next; }
	void setNext(Edge* he) { _next = he; };
	
	/**
	 * Gets / sets a pointer to the pair of this HEdge structure.
	 */
	Edge* pair() const { return _pair; };
	void setPair(Edge* he) { _pair = he; };

	/**
	 * Gets / sets a pointer to the vertex this structure points to.
	 */
	Vertex* head() const { return _head; };
	void setHead(Vertex* v) { _head = v; };
    
    /**
	 * Gets / sets a pointer to the vertex this structure points to.
	 */
	Vertex* tail() const { return _tail; };
	void setTail(Vertex* v) { _tail = v; };
	
	/**
	 * Gets / sets a pointer to a face this half edge belongs to.
	 */
	Face* face() const { return _face; };
	void setFace(Face* f) { _face = f; };
	
    bool isEqual(const Edge& he) const;
	bool operator==(const Edge& he) const;
	bool operator!=(const Edge& he) const;
	
	Iterator<Vertex> vertexIterator();
	
	Iterator<Edge> hedgeIterator();
	
	/* Inner classes */
public:
	class VertexIterator : public IIterator<Vertex>
	{
		friend class Edge;
		
		Edge* _he;
		Edge* _currHe;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		VertexIterator(Edge* he);
		
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
	
	class HEdgeIterator : public IIterator<Edge>
	{
		friend class Edge;
		
		Edge* _he;
		Edge* _currHe;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		HEdgeIterator(Edge* he);
		
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
		Edge & next();
		
		/**
		 * Returns the next element and advance the iterator by one.
		 */
		const Edge & next() const;
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		Edge & previous();
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		const Edge & previous() const;
	};
	
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

//inline bool operator==(const Edge &e1, const Edge &e2)
//{
//    return e1.isEqual(e2);
//}

inline uint qHash(const Edge& key)
{
//	if (key._head > key._tail)
//		return qHash( ((quint64)key._head) << 32 | (quint64)key._tail );
//	else
//		return qHash( ((quint64)key._tail) << 32 | (quint64)key._head );
    
    if (key._head->iid() > key._tail->iid())
		return qHash( ((quint64)key._head->iid()) << 32 |
                     (quint64)key._tail->iid() );
	else
		return qHash( ((quint64)key._tail->iid()) << 32 |
                     (quint64)key._head->iid() );
}

#endif
