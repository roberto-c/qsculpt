/*
 *  vector.h
 *  qsculpt
 *
 *  Created by Roberto Cabral on 2/25/11.
 *  Copyright 2011 plastilinaware. All rights reserved.
 *
 */

#ifndef DATAVECTOR_H_
#define DATAVECTOR_H_

#include <limits>
#include <PlastilinaCore/ICollection.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/Utilities.h>

namespace data {
    template <typename Type>
    class Vector;
}

namespace data {
    
    template< typename Type>
    class Vector: public ICollection<Type>
    {
        template<typename T>
        class TypeIterator;
        
    public:
        Vector() {
        }
        
        Vector(const Vector<Type>& cpy) : _data(cpy._data) { 
        }
        
        ~Vector() {
        }
        
        inline void add(const Type & e) { 
            _data.push_back(e); 
        }
        
        inline void remove(const Type & e) {
            _data.remove(_data.indexOf(e));
        }
        
        inline int size() {
            return _data.size();
        }
        
        bool findClosest(Type& out, const Point3& p, float d) const { 
            bool res = false;
            double l = std::numeric_limits<double>::max();
            Point3 r;
            for(auto e = _data.begin(); e != _data.end(); ++e) {
                r = (*e) - p;
                double d2 = r.norm();
                if ( d2 < l ) {
                    out = e;
                    l = d2;
                    res = true;
                }
            }
            return res;
        }
        
        inline bool findFirstIntersect(Type& out, const geometry::Ray& ray) const {
            for(auto e = _data.begin(); e != _data.end(); ++e) {
                float t = ray.intersect(*e, NULL);
                if (t >= 0 && t <= 1) {
                    out=e;
                    return true;
                }
            }
            return false;
        }
        
        inline bool findIntersect(const geometry::Ray&, 
                                  data::ICollection<Type>*) const { 
            throw std::runtime_error("Not implemented"); 
        }
        
        inline bool findIntersect(const geometry::AABB&, 
                                  data::ICollection<Type>*) const { 
            throw std::runtime_error("Not implemented"); 
        }
        
        inline Type* getData() {
            return _data.data();
        }
        
        inline Type& operator[](int index) {
            return _data[index];
        }
        
        inline const Type& operator[](int index) const {
            return _data[index];
        }
        
        inline const Type& at(int index) const {
            return _data.at(index);
        }
        
        inline std::string toString() {
            return std::string();
        }
        
        inline std::vector<Type> toStdVector() {
            return _data;
        }
        
        inline const std::vector<Type> toStdVector() const {
            return _data;
        }
        
        /**
         * Creates an iterator over the collection class
         */
        inline Iterator<Type> iterator() {
            return Iterator<Type>(new TypeIterator<Type>(this));
        }
        
        std::set<Type> toStdSet() {return std::set<Type>(); }
        
    private:
        
        
        // Do not allow coping semantincs
        Vector<Type>& operator=(const Vector<Type>&);
        
        std::vector<Type>      _data;
        
        template<typename T>
        class TypeIterator : public IIterator<T>
        {
            friend class data::Vector<T>;
            
            data::Vector<T> *_col;
            mutable int         _index;
        protected:
            /**
             * Constructor of a vertex iterator. The vertex iterator
             * is only contructed by means of Face::vertexIterator() function
             */
            TypeIterator(data::Vector<T> *col) : _col(col), _index(0) {}
            
        public:
            /**
             *
             */
            IIterator<T>* clone() const { 
                return new  TypeIterator<T>(*this);
            }
            
            /**
             * Return true if the iterator has more elements (i.e. it is not at the
             * end)
             */
            bool hasNext() const { 
                return _col->size() > 0 && _index < _col->size();
            }
            
            /**
             * Returns true if the iterator is not at the beginning of the iteration
             */
            bool hasPrevious() const {
                return _col->size() > 0 && _index < _col->size() && _index > 0;
            }
            
            /**
             * Returns the next element and advance the iterator by one.
             */
            T & next() {
                return _col->_data[_index++];
            }
            
            /**
             * Returns the next element and advance the iterator by one.
             */
            const T & next() const {
                return _col->_data.at(_index++);
            }
            
            /**
             * Returns the previous elements and move the iterator one position
             * backwards.
             */
            T & previous() {
                return _col->_data[--_index];
            }
            
            /**
             * Returns the previous elements and move the iterator one position
             * backwards.
             */
            const T & previous() const {
                return _col->_data.at(--_index);
            }
            
            /**
             * Set the current position to pos relative to origin.
             *
             * @param pos number of elements to jump relative to origin
             * @param origin states the reference to jump.
             */
            bool seek(int pos, IteratorOrigin origin) const {
                throw std::runtime_error("Not implemented");
            }
        };
    };
};

#endif
