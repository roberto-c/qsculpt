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
#include "stable.h"
#include "ClStlAllocator.h"
#include <PlastilinaCore/Logging.h>

namespace core
{
namespace cl
{

std::vector<MemoryPool*> g_poolList;

MemoryPool::MemoryPool(std::size_t n, int alignment) noexcept
: dataPtr(nullptr)
, size(n)
, locked(false)
{
    TRACE(debug) << "MemoryPool()";
    initialize();
}

MemoryPool::MemoryPool(const MemoryPool && pool)
: dataPtr(std::move(pool.dataPtr))
, size(std::move(pool.size))
, locked(std::move(pool.locked))
{
    TRACE(debug) << "MemoryPool(const MemoryPool && pool)";
}

MemoryPool::~MemoryPool()
{
    TRACE(debug) << "~MemoryPool()";
    delete dataPtr;
    dataPtr = nullptr;
}

bool
MemoryPool::initialize()
{
    if (dataPtr)
    {
        // already initialized
        return true;
    }
    dataPtr = new char[size];
    if (!dataPtr) return false;
    return true;
}

bool
MemoryPool::lock()
{
    locked = true;
    return true;
}

bool
MemoryPool::unlock()
{
    locked = false;
    return true;
}


MemoryPoolGpu::MemoryPoolGpu(
    ::cl::Context & ctx
    , ::cl::CommandQueue & queue
    , std::size_t n
    , cl_mem_flags flags)
    : MemoryPool(1)
    , queue(queue)
{
    TRACE(debug) << "MemoryPoolGpu()";
    
    context = ctx;
    buffer = ::cl::Buffer(context, flags|CL_MEM_ALLOC_HOST_PTR, n);
    size = n;
}

MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu & pool)
    : MemoryPool(pool)
    , context(pool.context)
    , queue(pool.queue)
    , buffer(pool.buffer)
{
    TRACE(trace) << "MemoryPoolGpu(const MemoryPoolGpu &)";
    NOT_IMPLEMENTED;
}


MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu && pool)
: MemoryPool(std::move(pool))
, context(std::move(pool.context))
, queue(std::move(pool.queue))
, buffer(std::move(pool.buffer))
{
    TRACE(trace) << "MemoryPoolGpu(const MemoryPoolGpu &&)";
}


MemoryPoolGpu::~MemoryPoolGpu()
{
    TRACE(trace) << "~MemoryPoolGpu()";
    if (dataPtr) {
        unlock();
    }
    dataPtr = nullptr;
}

bool
MemoryPoolGpu::lock()
{
    TRACE(trace) << "MemoryPoolGpu::lock()";
    if (locked) {
        TRACE(debug) << "Already locked";
        return true;
    }
    locked = true;
    dataPtr = static_cast<char*>(queue.enqueueMapBuffer(buffer,
        CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size));
    
    return dataPtr != nullptr;
}

bool
MemoryPoolGpu::unlock()
{
    TRACE(trace) << "MemoryPoolGpu::unlock()";
    if (!locked) {
        TRACE(debug) << "Not locked";
        return true;
    }
    queue.enqueueUnmapMemObject(buffer, dataPtr);
    //dataPtr = nullptr;
    locked = false;
    return true;
}

MemoryPoolGlCl::MemoryPoolGlCl(
    ::cl::Context & ctx
    , ::cl::CommandQueue & queue
    , std::size_t n
    , cl_mem_flags flags)
    : MemoryPool(1)
    , queue(queue)
{
    TRACE(trace) << "MemoryPoolGlCl()";
    context = ctx;
    gldata.create();
    gldata.setBufferData(nullptr, n);
    size = n;
    buffer = ::cl::BufferGL(context, flags, gldata.objectID());
}

MemoryPoolGlCl::MemoryPoolGlCl(const MemoryPoolGlCl & pool)
    : MemoryPool(pool)
    , context(pool.context)
    , queue(pool.queue)
    , buffer(pool.buffer)
    , gldata(pool.gldata)
{
    TRACE(trace) << "MemoryPoolGlCl(const MemoryPoolGlCl &)";
}


MemoryPoolGlCl::MemoryPoolGlCl(const MemoryPoolGlCl && pool)
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
    if (locked) {
        unlock();
    }
    dataPtr = nullptr;
}

bool
MemoryPoolGlCl::lock()
{
    TRACE(trace) << "MemoryPoolGlCl::lock()";
    if (locked) {
        TRACE(debug) << "Already locked";
        return true;
    }
    locked = true;
    //std::vector<::cl::Memory> list;
    //list.push_back(buffer);
    //std::vector<::cl::Event> evtList(1);
    //queue.enqueueAcquireGLObjects(&list, nullptr, &evtList[0]);
    //dataPtr = static_cast<char*>(queue.enqueueMapBuffer(buffer,
    //    CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size,&evtList));
    gldata.mapBuffer((GLvoid**)(&dataPtr), &size);

    return dataPtr != nullptr;
}

bool
MemoryPoolGlCl::unlock()
{
    TRACE(trace) << "MemoryPoolGlCl::unlock()";
    if (!locked) {
        TRACE(debug) << "Not locked";
        return true;
    }
    std::vector<::cl::Memory> list;
    list.push_back(buffer);
    //queue.enqueueUnmapMemObject(buffer, dataPtr);
    //queue.enqueueReleaseGLObjects(&list);
    gldata.unmapBuffer();
    locked = false;
    return true;
}


}; // namespace cl
}; // namspace core
