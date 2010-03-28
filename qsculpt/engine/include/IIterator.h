/*
 *  IIterator.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 1/7/10.
 *  Copyright 2010 Juan Roberto Cabral. All rights reserved.
 *
 */

#ifndef IITERATOR_H_
#define IITERATOR_H_

/**
 * Interface for iterator used in QSculpt
 */
template < typename T >
class IIterator
{
public:
	/**
	 * Return true if the iterator has more elements (i.e. it is not at the
	 * end)
	 */
	virtual bool hasNext() const = 0;
	
	/**
	 * Returns true if the iterator is not at the beginning of the iteration
	 */
	virtual bool hasPrevious() const = 0;
	
	/**
	 * Returns the next element and advance the iterator by one.
	 */
	virtual T & next() = 0;
	
	/**
	 * Returns the next element and advance the iterator by one.
	 */
	virtual const T & next() const = 0;
	
	/**
	 * Returns the previous elements and move the iterator one position
	 * backwards.
	 */
	virtual T & previous() = 0;
	
	/**
	 * Returns the previous elements and move the iterator one position
	 * backwards.
	 */
	virtual const T & previous() const = 0;
};


template< typename T>
class Iterator
{
	IIterator<T> *_it;
	
public:
	Iterator(IIterator<T>* it) : _it(it) {};
	
	/**
	 * Return true if the iterator has more elements (i.e. it is not at the
	 * end)
	 */
	inline bool hasNext() const { return _it->hasNext(); };
	
	/**
	 * Returns true if the iterator is not at the beginning of the iteration
	 */
	inline bool hasPrevious() const { return _it->hasPrevious(); };
	
	/**
	 * Returns the next element and advance the iterator by one.
	 */
	inline T & next() { return _it->next(); };
	
	/**
	 * Returns the next element and advance the iterator by one.
	 */
	inline const T & next() const { return _it->next(); };
	
	/**
	 * Returns the previous elements and move the iterator one position
	 * backwards.
	 */
	inline T & previous() { return _it->previous(); };
	
	/**
	 * Returns the previous elements and move the iterator one position
	 * backwards.
	 */
	inline const T & previous() const { return _it->previous(); };
};

#endif
