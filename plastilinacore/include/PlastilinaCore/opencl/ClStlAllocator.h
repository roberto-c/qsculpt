//
//  ClStlAllocator
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 6/13/15.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#include <PlastilinaCore/BufferObject.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <memory>
#include <new>

namespace core
{
namespace cl
{

class MemoryPool
{
  protected:
    char*       dataPtr;
    std::size_t size;
    bool        locked;

    virtual bool initialize(int alignment);

  public:
    MemoryPool(std::size_t n = 0, int alignment = 0) noexcept;

    MemoryPool(const MemoryPool&& pool);

    MemoryPool(const MemoryPool& other) = default;

    MemoryPool& operator=(MemoryPool&& other) = default;

    virtual ~MemoryPool();

    virtual bool lock();

    virtual bool unlock();

    void* data() const { return dataPtr; };
};

class MemoryPoolGpu : public MemoryPool
{
    ::cl::Context      context;
    ::cl::CommandQueue queue;
    ::cl::Buffer       buffer;

  public:
    /**
     * Create a memory pool backed by OpenCL memory object.
     */
    MemoryPoolGpu(::cl::Context& ctx, ::cl::CommandQueue& queue,
                  std::size_t n, cl_mem_flags flags);

    MemoryPoolGpu(const MemoryPoolGpu& other);

    MemoryPoolGpu(const MemoryPoolGpu&& pool);

    MemoryPoolGpu& operator=(MemoryPoolGpu&& other) = default;

    virtual ~MemoryPoolGpu();

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

    ::cl_mem& clMem() { return buffer(); }
    ::cl_mem  clMem() const { return buffer(); }
};

class MemoryPoolGlCl : public MemoryPool
{
    ::cl::Context      context;
    ::cl::CommandQueue queue;
    ::cl::Buffer       buffer;
    VertexBuffer       gldata;

  public:
    /**
     * Create a memory pool backed by OpenCL memory object.
     */
    MemoryPoolGlCl(::cl::Context& ctx, ::cl::CommandQueue& queue,
                   std::size_t n, cl_mem_flags flags);

    MemoryPoolGlCl(const MemoryPoolGlCl& other);

    MemoryPoolGlCl(const MemoryPoolGlCl&& pool);

    MemoryPoolGlCl& operator=(MemoryPoolGlCl&& other) = default;

    virtual ~MemoryPoolGlCl();

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

    ::cl_mem& clMem() { return buffer(); }
    ::cl_mem  clMem() const { return buffer(); }

    BufferObject& glBuffer() { return gldata; }

    const BufferObject& glBuffer() const { return gldata; }
};

extern std::vector<MemoryPool*> g_poolList;

struct SearchPtr
{
    const char* ptr;
    SearchPtr(const char* ptr = nullptr)
        : ptr(ptr)
    {
    }
    bool operator()(MemoryPool* pool) { return pool->data() == ptr; }
};

template <class T>
struct allocator
{

    typedef T value_type;

    allocator() noexcept
    {
        // TRACE(debug) << "allocator::allocator";
    }

    template <class U>
    allocator(const allocator<U>&) noexcept
    {
        // TRACE(debug) << "allocator::allocator";
    }

    T* allocate(std::size_t n)
    {
        TRACE(debug) << "allocastor::allocate size n = " << n;
        MemoryPool* pool = new MemoryPool(n * sizeof(T));
        pool->lock();
        g_poolList.push_back(pool);
        return static_cast<T*>(pool->data());
    }
    void deallocate(T* ptr, std::size_t n)
    {
        TRACE(debug) << "allocastor::deallocate size n = " << n;
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            delete *it;
            g_poolList.erase(it);
        }
        else
        {
            throw std::runtime_error(
                "Failed to deallocate. buffer not found");
        }
    }
};

template <class T, class U>
constexpr bool operator==(const allocator<T>&, const allocator<U>&) noexcept
{
    return true;
}

template <class T, class U>
constexpr bool operator!=(const allocator<T>&, const allocator<U>&) noexcept
{
    return false;
}

template <class T>
struct gpu_allocator
{

    typedef T value_type;

  private:
    ::cl::Context      ctx;
    ::cl::CommandQueue queue;

  public:
    gpu_allocator(::cl::Context& ctx, ::cl::CommandQueue& queue) noexcept
        : ctx(ctx)
        , queue(queue)
    {
        // TRACE(debug) << "gpu_allocator::gpu_allocator(ctx,queue)";
    }

    gpu_allocator() noexcept
    {
        // TRACE(debug) << "gpu_allocator::gpu_allocator()";
    }

    gpu_allocator(const gpu_allocator& cpy) noexcept
        : ctx(cpy.ctx)
        , queue(cpy.queue)
    {
        // TRACE(debug) << "gpu_allocator::gpu_allocator(cpy)";
    }

    template <class U>
    gpu_allocator(const gpu_allocator<U>& cpy) noexcept
        : ctx(cpy.ctx)
        , queue(cpy.queue)
    {
        // TRACE(debug) << "gpu_allocator::gpu_allocator(cpy)";
    }

    cl_mem backingStore(T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGpu* pool = static_cast<MemoryPoolGpu*>(*it);
            return pool->clMem();
        }
        throw std::runtime_error("opencl backing store not found");
    }

    cl_mem backingStore(const T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGpu* pool = static_cast<MemoryPoolGpu*>(*it);
            return (cl_mem)pool->clMem();
        }
        throw std::runtime_error("opencl backing store not found");
    }

    MemoryPool* memoryPool(T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGpu* pool = static_cast<MemoryPoolGpu*>(*it);
            return pool;
        }
        throw std::runtime_error("opencl backing store not found");
    }

    MemoryPool* memoryPool(const T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGpu* pool = static_cast<MemoryPoolGpu*>(*it);
            return pool;
        }
        throw std::runtime_error("opencl backing store not found");
    }

    T* allocate(std::size_t n)
    {
        TRACE(debug) << "gpu_allocator::allocate size n = " << n;
        MemoryPoolGpu* pool = new MemoryPoolGpu(ctx, queue, n * sizeof(T), 0);
        pool->lock();
        g_poolList.push_back(pool);
        return static_cast<T*>(pool->data());
    }
    void deallocate(T* ptr, std::size_t n)
    {
        TRACE(debug) << "gpu_allocator::deallocate size n = " << n;
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            delete *it;
            g_poolList.erase(it);
        }
        else
        {
            throw std::runtime_error(
                "Failed to deallocate. buffer not found");
        }
    }

    template <class U>
    friend struct gpu_allocator;
};

template <class T>
struct glgpu_allocator
{

    typedef T value_type;

  private:
    ::cl::Context      ctx;
    ::cl::CommandQueue queue;

  public:
    glgpu_allocator(::cl::Context& ctx, ::cl::CommandQueue& queue) noexcept
        : ctx(ctx)
        , queue(queue)
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator(ctx,queue)";
    }

    glgpu_allocator() noexcept
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator()";
    }

    glgpu_allocator(const glgpu_allocator& cpy) noexcept
        : ctx(cpy.ctx)
        , queue(cpy.queue)
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator(cpy&)";
    }

    glgpu_allocator(glgpu_allocator&& cpy) noexcept
        : ctx(std::move(cpy.ctx))
        , queue(std::move(cpy.queue))
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator(cpy&&)";
    }

    template <class U>
    glgpu_allocator(const glgpu_allocator<U>& cpy) noexcept
        : ctx(cpy.ctx)
        , queue(cpy.queue)
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator(cpy&)";
    }

    template <class U>
    glgpu_allocator(glgpu_allocator<U>&& cpy) noexcept
        : ctx(std::move(cpy.ctx))
        , queue(std::move(cpy.queue))
    {
        // TRACE(debug) << "glgpu_allocator::glgpu_allocator(cpy&&)";
    }

    cl_mem backingStore(T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return pool->clMem();
        }
        throw std::runtime_error("opencl backing store not found");
    }

    cl_mem backingStore(const T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return (cl_mem)pool->clMem();
        }
        throw std::runtime_error("opencl backing store not found");
    }

    ::BufferObject* glbackingStore(T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return &(pool->glBuffer());
        }
        throw std::runtime_error("opencl backing store not found");
    }

    ::BufferObject* glbackingStore(const T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return &(pool->glBuffer());
        }
        throw std::runtime_error("opencl backing store not found");
    }

    MemoryPool* memoryPool(T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return pool;
        }
        throw std::runtime_error("opencl backing store not found");
    }

    MemoryPool* memoryPool(const T* ptr)
    {
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            MemoryPoolGlCl* pool = static_cast<MemoryPoolGlCl*>(*it);
            return pool;
        }
        throw std::runtime_error("opencl backing store not found");
    }

    T* allocate(std::size_t n)
    {
        TRACE(debug) << "glgpu_allocator::allocate size n = " << n;
        MemoryPoolGlCl* pool =
            new MemoryPoolGlCl(ctx, queue, n * sizeof(T), CL_MEM_READ_WRITE);
        pool->lock();
        g_poolList.push_back(pool);
        return static_cast<T*>(pool->data());
    }
    void deallocate(T* ptr, std::size_t n)
    {
        TRACE(debug) << "glgpu_allocator::deallocate size n = " << n;
        auto it = std::find_if(g_poolList.begin(), g_poolList.end(),
                               SearchPtr((const char*)ptr));
        if (it != g_poolList.end())
        {
            delete *it;
            g_poolList.erase(it);
        }
        else
        {
            throw std::runtime_error(
                "Failed to deallocate. buffer not found");
        }
    }

    template <class U>
    friend struct glgpu_allocator;
};

template <typename T>
cl_mem get_mem_backing_store(const T& container)
{
    return container.get_allocator().backingStore(container.data());
}

template <typename T>
BufferObject* get_glbuffer_backing_store(const T& container)
{
    return container.get_allocator().glbackingStore(container.data());
}

template <typename T>
void lock_container(const T& container)
{
    container.get_allocator().memoryPool(container.data())->lock();
}

template <typename T>
void unlock_container(const T& container)
{
    container.get_allocator().memoryPool(container.data())->unlock();
}

}; // namespace cl
}; // namespace core
