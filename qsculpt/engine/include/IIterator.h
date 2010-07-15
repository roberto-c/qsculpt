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


// TODO: Check if it the need to optimize iterator design. To many indirections?

/**
 * Used for the seek method in the iterators.
 */
enum IteratorOrigin
{
    Iter_Start,
    Iter_Current,
    Iter_End
};

/**
 * Interface for iterator used in QSculpt
 */
template< typename T >
class IIterator
{
public:
    //virtual ~IIterator(){}

    /**
     * Function to copy iterators of the same type.
     */
    virtual IIterator<T>* clone() const = 0;

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

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const = 0;
};


/**
 * Wrapper class around classes derived from IIterator.
 *
 * Offers copy / clone semantics between iterators. It is a small class
 * and is meant to be be used as a variables stored in the stack.
 */
template< typename T>
class Iterator
{
    IIterator<T> *_it;

public:
    /**
     * Iterator constructor. Sets up this iterator wrapper around the
     * passed iterator implementation class.
     */
    Iterator(IIterator<T>* it) : _it(it) {}

    /**
     * Contructor to make a copy of another iterator.
     */
    Iterator(const Iterator<T>& cpy) : _it(cpy._it->clone()) {}

    /**
     * Deletes the iterator implementation instance.
     */
    ~Iterator() { delete _it; _it = NULL;}

    /**
     * Assignment operator.
     */
    Iterator& operator=(const Iterator<T>& it) {
        if (this == &it) return *this;

        this->_it = it._it->clone();
        return *this;
    }

    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    inline bool hasNext() const { return _it->hasNext(); }

    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    inline bool hasPrevious() const { return _it->hasPrevious(); }

    /**
     * Returns the next element and advance the iterator by one.
     */
    inline T & next() { return _it->next(); }

    /**
     * Returns the next element and advance the iterator by one.
     */
    inline const T & next() const { return _it->next(); }

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    inline T & previous() { return _it->previous(); }

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    inline const T & previous() const { return _it->previous(); }

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    inline bool seek(int pos, IteratorOrigin origin = Iter_Start) const {
        return _it->seek(pos, origin);
    }
};

#endif
