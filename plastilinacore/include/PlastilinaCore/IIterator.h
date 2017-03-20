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

#include <PlastilinaCore/Plastilina.h>

// TODO: Check if it the need to optimize iterator design. To many
// indirections?

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
template <typename T>
class IIterator
{
  public:
    typedef typename T::shared_ptr       shared_ptr;
    typedef typename T::const_shared_ptr const_shared_ptr;
    typedef typename T::weak_ptr         weak_ptr;
    typedef typename T::ptr              ptr;
    typedef typename T::const_ptr        const_ptr;

  public:
    virtual ~IIterator() {}

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
    virtual shared_ptr next() = 0;

    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const_shared_ptr next() const = 0;

    /**
     * Return the next element without advancing to the iterator
     */
    virtual shared_ptr peekNext(){NOT_IMPLEMENTED};

    /**
     * Return the next element without advancing to the iterator
     */
    virtual const_shared_ptr peekNext() const {NOT_IMPLEMENTED};

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual shared_ptr previous() = 0;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const_shared_ptr previous() const = 0;

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const = 0;

    virtual bool operator==(const IIterator& rhs) { return false; }

    virtual bool operator!=(const IIterator& rhs) { return false; }
};

/**
 * Wrapper class around classes derived from IIterator.
 *
 * Offers copy / clone semantics between iterators. It is a small class
 * and is meant to be be used as a variables stored in the stack.
 */
template <typename T>
class Iterator
{
  public:
    typedef typename T::shared_ptr       shared_ptr;
    typedef typename T::const_shared_ptr const_shared_ptr;
    typedef typename T::weak_ptr         weak_ptr;
    typedef typename T::ptr              ptr;
    typedef typename T::const_ptr        const_ptr;

  private:
    std::unique_ptr<IIterator<T>> _it;

  public:
    /**
     * Iterator constructor. Sets up this iterator wrapper around the
     * passed iterator implementation class.
     */
    Iterator(IIterator<T>* it = 0)
        : _it(it)
    {
    }

    /**
     * Contructor to make a copy of another iterator.
     */
    Iterator(const Iterator<T>& cpy)
        : _it(cpy._it->clone())
    {
    }

    /**
     * Contructor to make a copy of another iterator.
     */
    Iterator(Iterator<T>&& cpy) = default;

    /**
     * Deletes the iterator implementation instance.
     */
    ~Iterator() {}

    /**
     * Assignment operator.
     */
    Iterator& operator=(const Iterator<T>& it)
    {
        if (this == &it)
            return *this;

        this->_it = std::unique_ptr<IIterator<T>>(it._it->clone());
        return *this;
    }

    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    inline bool hasNext() const { return _it && _it->hasNext(); }

    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    inline bool hasPrevious() const { return _it && _it->hasPrevious(); }

    /**
     * Returns the next element and advance the iterator by one.
     */
    inline shared_ptr next()
    {
        assert(_it != nullptr);
        return _it->next();
    }

    /**
     * Returns the next element and advance the iterator by one.
     */
    inline const_shared_ptr next() const
    {
        assert(_it != 0);
        return _it->next();
    }

    /**
     * Return the next element without advancing to the iterator
     */
    inline shared_ptr peekNext()
    {
        assert(_it != 0);
        return _it->peekNext();
    }

    /**
     * Return the next element without advancing to the iterator
     */
    inline const_shared_ptr peekNext() const
    {
        assert(_it != 0);
        return _it->peekNext();
    }

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    inline shared_ptr previous()
    {
        assert(_it != 0);
        return _it->previous();
    }

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    inline const_shared_ptr previous() const
    {
        assert(_it != 0);
        return _it->previous();
    }

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    inline bool seek(int pos, IteratorOrigin origin = Iter_Start) const
    {
        assert(_it != 0);
        return _it->seek(pos, origin);
    }

    /**
     * Implementation to use for STL algorithms
     */
    inline Iterator begin() { return *this; }

    /**
     * Implementation to use for STL algorithms
     */
    inline Iterator end()
    {
        assert(_it != 0);
        Iterator<T> tmp(*this);
        tmp.seek(0, Iter_End);
        return tmp;
    }

    /**
     * Implementation to use for STL algorithms
     */
    inline Iterator& operator++()
    {
        assert(_it != 0);
        next();
        return *this;
    }

    /**
     * Implementation to use for STL algorithms
     */
    inline Iterator operator++(int)
    {
        assert(_it != 0);
        Itarator<T> tmp(*this);
        ++(*this);
        return tmp;
    }

    /**
     * Implementation to use for STL algorithms
     */
    inline T& operator*()
    {
        assert(_it != 0);
        return *peekNext();
    }

    /**
     * Implementation to use for STL algorithms
     */
    friend bool operator==(const Iterator& lhs, const Iterator& rhs)
    {
        assert(lhs._it != 0 && rhs._it);
        if (&lhs == &rhs)
            return true;
        return lhs._it->operator==(*(rhs._it));
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
    {
        return !(lhs == rhs);
    }
};

#endif
