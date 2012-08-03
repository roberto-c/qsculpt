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
#include "Stable.h"
#include "BOManager.h"
#include "ISurface.h"

// TODO: Mechanism to invalidate buffer objects.

BOManager* BOManager::m_instance = NULL;

BOManager::BOManager()
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
    BOMeshMap::iterator itEnd = m_boMeshMap.end();
    for (BOMeshMap::iterator it = m_boMeshMap.begin(); it != itEnd; ++it)
    {
        if (mesh == (*it).second)
        {
            m_boMap[(*it).first]->setNeedUpdate(true);
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
            m_boMeshMap[bo->getBufferID()] = mesh;
            m_vboPool[poolName][mesh]= bo->getBufferID();
            m_boMap[bo->getBufferID()] = bo;
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
            m_boMeshMap[bo->getBufferID()] = mesh;
            m_iboPool[poolName][mesh]= bo->getBufferID();
            m_boMap[bo->getBufferID()] = bo;
        }
    }
    return bo;
}

void BOManager::destroyBO(const std::string& poolName, BufferObject* bo)
{
    assert(bo);
    if (bo->getBufferID() != 0 && m_boMeshMap.find(bo->getBufferID()) != m_boMeshMap.end())
    {
        ISurface* mesh = getMesh(bo);
        if (mesh)
        {
            if (bo->getType() == GL_ARRAY_BUFFER)
                m_vboPool[poolName].erase(mesh);
            else
                m_iboPool[poolName].erase(mesh);
        }
        m_boMeshMap.erase(bo->getBufferID());
        m_boMap.erase(bo->getBufferID());
        bo->destroy();
        delete bo;
        bo = NULL;
    }
}

void BOManager::destroyAllMeshBO(const ISurface* mesh)
{
    assert(mesh);
    BOMeshMap::iterator itEnd = m_boMeshMap.end();
    for (BOMeshMap::iterator it = m_boMeshMap.begin(); it != itEnd; ++it)
    {
        if ((*it).second == mesh)
        {
            m_boMeshMap.erase((*it).first);
        }
    }
}

void BOManager::destroyPool(const std::string& poolName)
{
    std::vector<GLuint> boId;

    if (m_vboPool.find(poolName) != m_vboPool.end())
    {
        MeshBOMap::iterator itEnd = m_vboPool[poolName].end();
        for (auto it = m_vboPool[poolName].begin();  it != itEnd; ++it)
        {
            boId.push_back((*it).second);
        }
    }
    if (m_iboPool.find(poolName) != m_iboPool.end())
    {
        MeshBOMap::iterator itEnd = m_iboPool[poolName].end();
        for (auto it = m_iboPool[poolName].begin(); it != itEnd; ++it)
        {
            boId.push_back((*it).second);
        }
    }

    for (int i = 0; i < boId.size(); ++i)
    {
        auto bo = m_boMap.find(boId[i]);
        if (bo != m_boMap.end()) {
            destroyBO(poolName, (*bo).second);
        }
    }

}

ISurface* BOManager::getMesh(const BufferObject* bo)
{
    assert(bo);
    if (bo->getBufferID() != 0 && m_boMeshMap.find(bo->getBufferID()) != m_boMeshMap.end())
    {
        return const_cast<ISurface*>(m_boMeshMap[bo->getBufferID()]);
    }
    return NULL;
}

VertexBuffer* BOManager::getVBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    if (m_vboPool.find(poolName) != m_vboPool.end()
        && m_vboPool[poolName].find(mesh) != m_vboPool[poolName].end())
    {
        return reinterpret_cast<VertexBuffer*>(m_boMap[m_vboPool[poolName].at(mesh)]);
    }
    return NULL;
}

IndexBuffer* BOManager::getIBO(const std::string& poolName, const ISurface* mesh)
{
    assert(mesh);
    if (m_iboPool.find(poolName) != m_iboPool.end()
        && m_iboPool[poolName].find(mesh) != m_iboPool[poolName].end())
        {
        return reinterpret_cast<IndexBuffer*>(m_boMap[m_iboPool[poolName].at(mesh)]);
    }
    return NULL;
}

