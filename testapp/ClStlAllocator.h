//
//  ClStlAllocator
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 6/13/15.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#include <PlastilinaCore/opencl/cl.hpp>
#include <new>
#include <memory>

namespace core {
namespace cl {
    class MemoryPool {
        ::cl::Buffer buffer;
    public:
        /**
         *
         */
        MemoryPool(::cl::Context ctx, std::size_t n, cl_mem_flags flags) noexcept;
        
        /**
         * Lock the memory pool. 
         *
         * This will cause the host to read the memory from the device by
         * mapping it into a host pointer. All pointers obtained from this pool
         * while the memory was not mapped is invalid.
         */
        bool lock();
        
        /**
         * Unlock the memory pool.
         *
         * This will cause the host to write the memory to the device by
         * unmapping it. All pointers obtained from this pool will become invalid
         */
        bool unlock();
        
        ::cl::Buffer & data() { return buffer; }
        const ::cl::Buffer & data() const { return buffer; }
    };
    template <class T>
    struct allocator {
        typedef T value_type;
        
        
        allocator() noexcept {}
        
        template <class U> allocator (const allocator<U>&) noexcept {}
        
        T* allocate (std::size_t n) {
            std::cerr << "Allocastor::allocate size n = " << n << "\n";
            return static_cast<T*>( operator new( n*sizeof(T) ) );
        }
        void deallocate (T* p, std::size_t n) {
            std::cerr << "Allocastor::deallocate size n = " << n << "\n";
            ::delete(p);
        }
    };
    
    template <class T, class U>
    constexpr bool operator== (const allocator<T>&, const allocator<U>&) noexcept
    {return true;}
    
    template <class T, class U>
    constexpr bool operator!= (const allocator<T>&, const allocator<U>&) noexcept
    {return false;}

};
};
