/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores   *
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
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/ISurface.h>

// TODO: Mechanism to invalidate buffer objects.

struct BOManager::Impl
{
    IdBufferObjectMap   m_boMap;
    IdVaoMap            m_vaoMap;
	BOPool              m_vboPool;
	BOPool              m_iboPool;
    BOPool              m_vaoPool;
	BOMeshMap           m_boMeshMap;
};

BOManager* BOManager::m_instance = NULL;

BOManager::BOManager()
: d(new Impl)
{
}

BOManager::~BOManager()
{
}

BOManager* BOManager::getInstance()
{
    if (m_instance == NULL)
        m_instance = new BOManager;

    return m_instance;
}

void BOManager::invalidateBO(ISurface* mesh)
{
    BOMeshMap::iterator itEnd = d->m_boMeshMap.end();
    for (BOMeshMap::iterator it = d->m_boMeshMap.begin(); it != itEnd; ++it)
    {
        if (mesh == (*it).second)
        {
            d->m_boMap[(*it).first]->setNeedUpdate(true);
            //std::cerr << " update BOs";
        }
    }
}

VertexBuffer* BOManager::createVBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    VertexBuffer* bo = new VertexBuffer;
    if (bo)
    {
        if (!bo->create())
        {
            delete bo;
            bo = NULL;
        }
        else
        {
            d->m_boMeshMap[bo->objectID()] = mesh;
            d->m_vboPool[poolName][mesh]= bo->objectID();
            d->m_boMap[bo->objectID()] = bo;
        }
    }
    return bo;
}

IndexBuffer* BOManager::createIBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    IndexBuffer* bo = new IndexBuffer;
    if (bo)
    {
        if (!bo->create())
        {
            delete bo;
            bo = NULL;
        }
        else
        {
            d->m_boMeshMap[bo->objectID()] = mesh;
            d->m_iboPool[poolName][mesh]= bo->objectID();
            d->m_boMap[bo->objectID()] = bo;
        }
    }
    return bo;
}

VAO* BOManager::createVAO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    VAO* bo = new VAO;
    if (bo)
    {
        d->m_boMeshMap[bo->objectID()] = mesh;
        d->m_vaoPool[poolName][mesh]= bo->objectID();
        d->m_vaoMap[bo->objectID()] = bo;
    }
    return bo;
}

void BOManager::destroyBO(const std::string& poolName, BufferObject* bo)
{
    assert(bo);
    if (bo->objectID() != 0 &&
        d->m_boMeshMap.find(bo->objectID()) != d->m_boMeshMap.end())
    {
        ISurface* mesh = getMesh(bo);
        if (mesh)
        {
            if (bo->getType() == GL_ARRAY_BUFFER)
                d->m_vboPool[poolName].erase(mesh);
            else
                d->m_iboPool[poolName].erase(mesh);
        }
        d->m_boMeshMap.erase(bo->objectID());
        d->m_boMap.erase(bo->objectID());
        bo->destroy();
        delete bo;
        bo = NULL;
    }
}

void BOManager::destroyBO(const std::string& poolName, VAO* bo)
{
    assert(bo);
    if (bo->objectID() != 0 &&
        d->m_boMeshMap.find(bo->objectID()) != d->m_boMeshMap.end())
    {
        ISurface* mesh = getMesh(bo);
        if (mesh)
        {
            d->m_vaoPool[poolName].erase(mesh);
        }
        d->m_boMeshMap.erase(bo->objectID());
        d->m_vaoMap.erase(bo->objectID());
        delete bo;
    }
}


void BOManager::destroyAllMeshBO(const ISurface* mesh)
{
    assert(mesh);
    BOMeshMap::iterator itEnd = d->m_boMeshMap.end();
    for (BOMeshMap::iterator it = d->m_boMeshMap.begin(); it != itEnd; ++it)
    {
        if ((*it).second == mesh)
        {
            d->m_boMeshMap.erase((*it).first);
        }
    }
}

void BOManager::destroyPool(const std::string& poolName)
{
    std::vector<GLuint> boId;

    if (d->m_vboPool.find(poolName) != d->m_vboPool.end())
    {
        MeshBOMap::iterator itEnd = d->m_vboPool[poolName].end();
        for (auto it = d->m_vboPool[poolName].begin();  it != itEnd; ++it)
        {
            boId.push_back((*it).second);
        }
    }
    if (d->m_iboPool.find(poolName) != d->m_iboPool.end())
    {
        MeshBOMap::iterator itEnd = d->m_iboPool[poolName].end();
        for (auto it = d->m_iboPool[poolName].begin(); it != itEnd; ++it)
        {
            boId.push_back((*it).second);
        }
    }

    for (unsigned int i = 0; i < boId.size(); ++i)
    {
        auto bo = d->m_boMap.find(boId[i]);
        if (bo != d->m_boMap.end()) {
            destroyBO(poolName, (*bo).second);
        }
    }
    boId.clear();
    
    if (d->m_vaoPool.find(poolName) != d->m_vaoPool.end())
    {
        MeshBOMap::iterator itEnd = d->m_vaoPool[poolName].end();
        for (auto it = d->m_vaoPool[poolName].begin(); it != itEnd; ++it)
        {
            boId.push_back((*it).second);
        }
    }
    for (unsigned int i = 0; i < boId.size(); ++i)
    {
        auto bo = d->m_vaoMap.find(boId[i]);
        if (bo != d->m_vaoMap.end()) {
            destroyBO(poolName, (*bo).second);
        }
    }
}

ISurface* BOManager::getMesh(const BufferObject* bo)
{
    assert(bo);
    if (bo->objectID() != 0 &&
        d->m_boMeshMap.find(bo->objectID()) != d->m_boMeshMap.end())
    {
        return const_cast<ISurface*>(d->m_boMeshMap[bo->objectID()]);
    }
    return NULL;
}

ISurface* BOManager::getMesh(const VAO* bo)
{
    assert(bo);
    if (bo->objectID() != 0 &&
        d->m_boMeshMap.find(bo->objectID()) != d->m_boMeshMap.end())
    {
        return const_cast<ISurface*>(d->m_boMeshMap[bo->objectID()]);
    }
    return NULL;
}


VertexBuffer* BOManager::getVBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    if (d->m_vboPool.find(poolName) != d->m_vboPool.end()
        && d->m_vboPool[poolName].find(mesh) != d->m_vboPool[poolName].end())
    {
        return reinterpret_cast<VertexBuffer*>(d->m_boMap[d->m_vboPool[poolName].at(mesh)]);
    }
    return NULL;
}

IndexBuffer* BOManager::getIBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    if (d->m_iboPool.find(poolName) != d->m_iboPool.end()
        && d->m_iboPool[poolName].find(mesh) != d->m_iboPool[poolName].end())
        {
        return reinterpret_cast<IndexBuffer*>(d->m_boMap[d->m_iboPool[poolName].at(mesh)]);
    }
    return NULL;
}

VAO* BOManager::getVAO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    auto pool = d->m_vaoPool.find(poolName);
    if (pool == d->m_vaoPool.end()) {
        return NULL;
    }
    if (pool->second.find(mesh) != pool->second.end())
    {
        return reinterpret_cast<VAO*>(d->m_vaoMap[pool->second.at(mesh)]);
    }
    return NULL;
}