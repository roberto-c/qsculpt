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

namespace core
{
namespace cl
{

std::vector<MemoryPool*> g_poolList;

MemoryPool::MemoryPool(std::size_t n, int alignment) noexcept
: dataPtr(nullptr)
, size(n)
{
    std::cerr << "MemoryPool()\n";
    initialize();
}

MemoryPool::MemoryPool(const MemoryPool && pool)
: dataPtr(std::move(pool.dataPtr))
, size(std::move(pool.size))
{
}

MemoryPool::~MemoryPool()
{
    std::cerr << "~MemoryPool()\n";
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
    return true;
}

bool
MemoryPool::unlock()
{
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
    std::cerr << "MemoryPoolGpu()\n";
    
    context = ctx;
    buffer = ::cl::Buffer(context, flags|CL_MEM_ALLOC_HOST_PTR, n);
    size = n;
}

MemoryPoolGpu::MemoryPoolGpu(
    ::cl::Context & ctx
    , ::cl::CommandQueue & queue
    , BufferObject & bufobj
    , cl_mem_flags flags ) 
    : MemoryPool(1)
    , queue(queue)
{
    std::cerr << "MemoryPoolGpu()\n";
    context = ctx;
    size = bufobj.getBufferSize();
    buffer = ::cl::BufferGL(context, flags, bufobj.objectID());
}

MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu & pool)
    : MemoryPool(pool)
    , context(pool.context)
    , queue(pool.queue)
    , buffer(pool.buffer)
{
    NOT_IMPLEMENTED;
}


MemoryPoolGpu::MemoryPoolGpu(const MemoryPoolGpu && pool)
: MemoryPool(std::move(pool))
, context(std::move(pool.context))
, queue(std::move(pool.queue))
, buffer(std::move(pool.buffer))
{
}


MemoryPoolGpu::~MemoryPoolGpu()
{
    std::cerr << "~MemoryPoolGpu()\n";
    if (dataPtr) {
        unlock();
    }
}

bool
MemoryPoolGpu::lock()
{
    dataPtr = static_cast<char*>(queue.enqueueMapBuffer(buffer,
        CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size));
    
    return dataPtr != nullptr;
}

bool
MemoryPoolGpu::unlock()
{
    queue.enqueueUnmapMemObject(buffer, dataPtr);
    dataPtr = nullptr;
    return true;
}

}; // namespace cl
}; // namspace core
