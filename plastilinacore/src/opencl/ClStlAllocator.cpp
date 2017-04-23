/***************************************************************************
 *   Copyright (C) 2015 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/opencl/ClStlAllocator.h>
#include <stdlib.h>

namespace core
{
namespace cl
{

std::vector<MemoryPool*> g_poolList;

MemoryPool::MemoryPool(std::size_t n, int alignment) noexcept
    : dataPtr(nullptr)
    , rawDataPtr(nullptr)
    , size(n)
    , locked(false)
{
    TRACE(debug) << "MemoryPool()";
    initialize(alignment);
}

MemoryPool::MemoryPool(const MemoryPool&& pool)
    : dataPtr(std::move(pool.dataPtr))
    , rawDataPtr(std::move(pool.rawDataPtr))
    , size(std::move(pool.size))
    , locked(std::move(pool.locked))
{
    TRACE(debug) << "MemoryPool(const MemoryPool && pool)";
}

MemoryPool::~MemoryPool()
{
    TRACE(debug) << "~MemoryPool()";
	dataPtr = nullptr;
    delete[] rawDataPtr;
	rawDataPtr = nullptr;
}

bool MemoryPool::initialize(int alignment)
{
    if (dataPtr)
    {
        // already initialized
        return true;
    }
    size_t sizeElements = size % alignof(std::max_align_t) == 0
                              ? size / alignof(std::max_align_t)
                              : (size / alignof(std::max_align_t)) + 1;
    rawDataPtr = new std::max_align_t[sizeElements];
    if (!rawDataPtr)
        return false;
	size_t space = sizeElements * alignof(std::max_align_t);
	dataPtr = std::align(alignof(std::max_align_t), size, rawDataPtr, space);
    return true;
}

bool MemoryPool::lock()
{
    locked = true;
    return true;
}

bool MemoryPool::unlock()
{
    locked = false;
    return true;
}

/////////////////////////////////////////////////////////////////////////////

MemoryPoolGpu::MemoryPoolGpu(::cl::Context& ctx, ::cl::CommandQueue& queue,
                             std::size_t n, cl_mem_flags flags)
    : MemoryPool(n)
    , queue(queue)
{
    TRACE(debug) << "MemoryPoolGpu()";

    context = ctx;
    buffer  = ::cl::Buffer(context, flags | CL_MEM_USE_HOST_PTR, n, dataPtr);
    size    = n;
}

MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu& pool)
    : MemoryPool(pool)
    , context(pool.context)
    , queue(pool.queue)
    , buffer(pool.buffer)
{
    TRACE(trace) << "MemoryPoolGpu(const MemoryPoolGpu &)";
    NOT_IMPLEMENTED;
}

MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu&& pool)
    : MemoryPool(std::move(pool))
    , context(std::move(pool.context))
    , queue(std::move(pool.queue))
    , buffer(std::move(pool.buffer))
{
    TRACE(trace) << "MemoryPoolGpu(const MemoryPoolGpu &&)";
}

//MemoryPoolGpu& MemoryPool::operator=(MemoryPoolGpu&& other)
//{
//    TRACE(trace) << "MemoryPoolGpu(const MemoryPoolGpu &&)";
//}

MemoryPoolGpu::~MemoryPoolGpu()
{
    TRACE(trace) << "~MemoryPoolGpu()";
    if (dataPtr)
    {
        unlock();
    }
}

bool MemoryPoolGpu::lock()
{
    TRACE(trace) << "MemoryPoolGpu::lock()";
    if (locked)
    {
        TRACE(warning) << "MemoryPoolGpu::lock(): Already locked";
        return true;
    }
    locked         = true;
    char* dataPtr2 = static_cast<char*>(queue.enqueueMapBuffer(
        buffer, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size));
    assert(dataPtr2 == dataPtr);
    return dataPtr != nullptr;
}

bool MemoryPoolGpu::unlock()
{
    TRACE(trace) << "MemoryPoolGpu::unlock()";
    if (!locked)
    {
        TRACE(warning) << "MemoryPoolGpu::unlock(): Not locked";
        return true;
    }
    queue.enqueueUnmapMemObject(buffer, dataPtr);
    locked = false;
    return true;
}

/////////////////////////////////////////////////////////////////////////////

MemoryPoolGlCl::MemoryPoolGlCl(::cl::Context& ctx, ::cl::CommandQueue& queue,
                               std::size_t n, cl_mem_flags flags)
    : MemoryPool(n)
    , queue(queue)
{
    TRACE(trace) << "MemoryPoolGlCl()";
    context = ctx;
    gldata.create();
    gldata.setBufferData(dataPtr, n);
    size   = n;
    buffer = ::cl::BufferGL(context, flags, gldata.objectID());
}

MemoryPoolGlCl::MemoryPoolGlCl(const MemoryPoolGlCl& pool)
    : MemoryPool(pool)
    , context(pool.context)
    , queue(pool.queue)
    , buffer(pool.buffer)
    , gldata(pool.gldata)
{
    TRACE(trace) << "MemoryPoolGlCl(const MemoryPoolGlCl &)";
}

MemoryPoolGlCl::MemoryPoolGlCl(const MemoryPoolGlCl&& pool)
    : MemoryPool(std::move(pool))
    , context(std::move(pool.context))
    , queue(std::move(pool.queue))
    , buffer(std::move(pool.buffer))
    , gldata(std::move(pool.gldata))
{
    TRACE(trace) << "MemoryPoolGlCl(const MemoryPoolGlCl &&)";
}

MemoryPoolGlCl::~MemoryPoolGlCl()
{
    TRACE(trace) << "~MemoryPoolGlCl()";
    if (locked)
    {
        unlock();
    }
}

bool MemoryPoolGlCl::lock()
{
    TRACE(trace) << "MemoryPoolGlCl::lock()";
    if (locked)
    {
        TRACE(debug) << "Already locked";
        return true;
    }
    locked = true;
    std::vector<::cl::Memory> list;
    list.push_back(buffer);
    std::vector<::cl::Event> evtList(1);
    queue.enqueueAcquireGLObjects(&list, nullptr, &evtList[0]);
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, size, dataPtr, &evtList);
    queue.enqueueReleaseGLObjects(&list);
    queue.flush();
    return dataPtr != nullptr;
}

bool MemoryPoolGlCl::unlock()
{
    TRACE(trace) << "MemoryPoolGlCl::unlock()";
    if (!locked)
    {
        TRACE(debug) << "Not locked";
        return true;
    }
    std::vector<::cl::Memory> list;
    list.push_back(buffer);
    std::vector<::cl::Event> evtList(1);
    queue.enqueueAcquireGLObjects(&list, nullptr, &evtList[0]);
    queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, dataPtr);
    queue.enqueueReleaseGLObjects(&list);
    queue.flush();
    locked = false;
    return true;
}

}; // namespace cl
}; // namspace core
