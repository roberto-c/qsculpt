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
#include "StdAfx.h"
#include "BOManager.h"
#include "ISurface.h"

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
        if (mesh == it.value())
        {
            m_boMap[it.key()]->setNeedUpdate(true);
            //qDebug() << " update BOs";
        }
    }
}

VertexBuffer* BOManager::createVBO(const QString& poolName, const ISurface* mesh)
{
    Q_ASSERT(mesh);
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

            connect(mesh, SIGNAL(meshChanged(ISurface*)),
                    this, SLOT(invalidateBO(ISurface*)));
        }
    }
    return bo;
}

IndexBuffer* BOManager::createIBO(const QString& poolName, const ISurface* mesh)
{
    Q_ASSERT(mesh);
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

void BOManager::destroyBO(const QString& poolName, BufferObject* bo)
{
    Q_ASSERT(bo);
    if (bo->getBufferID() != 0 && m_boMeshMap.contains(bo->getBufferID()))
    {
        ISurface* mesh = getMesh(bo);
        if (mesh)
        {
            if (bo->getType() == GL_ARRAY_BUFFER)
                m_vboPool[poolName].remove(mesh);
            else
                m_iboPool[poolName].remove(mesh);
        }
        m_boMeshMap.remove(bo->getBufferID());
        m_boMap.remove(bo->getBufferID());
        bo->destroy();
        delete bo;
        bo = NULL;
    }
}

void BOManager::destroyAllMeshBO(const ISurface* mesh)
{
    Q_ASSERT(mesh);
    BOMeshMap::iterator itEnd = m_boMeshMap.end();
    for (BOMeshMap::iterator it = m_boMeshMap.begin(); it != itEnd; ++it)
    {
        if (it.value() == mesh)
        {
            m_boMeshMap.remove(it.key());
        }
    }
}

void BOManager::destroyPool(const QString& poolName)
{
    QVector<GLuint> boId;

    if (m_vboPool.contains(poolName))
    {
        MeshBOMap::iterator itEnd = m_vboPool[poolName].end();
        for (MeshBOMap::iterator it = m_vboPool[poolName].begin();
        it != itEnd; ++it)
        {
            boId.append(it.value());
        }
    }
    if (m_iboPool.contains(poolName))
    {
        MeshBOMap::iterator itEnd = m_iboPool[poolName].end();
        for (MeshBOMap::iterator it = m_iboPool[poolName].begin();
        it != itEnd; ++it)
        {
            boId.append(it.value());
        }
    }

    BufferObject *bo = NULL;
    for (int i = 0; i < boId.count(); ++i)
    {
        bo = m_boMap.value(boId[i], NULL);
        if (bo)
            destroyBO(poolName, bo);
    }

}

ISurface* BOManager::getMesh(const BufferObject* bo)
{
    Q_ASSERT(bo);
    if (bo->getBufferID() != 0 && m_boMeshMap.contains(bo->getBufferID()))
    {
        return const_cast<ISurface*>(m_boMeshMap[bo->getBufferID()]);
    }
    return NULL;
}

VertexBuffer* BOManager::getVBO(const QString& poolName, const ISurface* mesh)
{
    Q_ASSERT(mesh);
    if (m_vboPool.contains(poolName)
        && m_vboPool[poolName].contains(mesh))
        {
        return reinterpret_cast<VertexBuffer*>(m_boMap[m_vboPool[poolName].value(mesh)]);
    }
    return NULL;
}

IndexBuffer* BOManager::getIBO(const QString& poolName, const ISurface* mesh)
{
    Q_ASSERT(mesh);
    if (m_iboPool.contains(poolName)
        && m_iboPool[poolName].contains(mesh))
        {
        return reinterpret_cast<IndexBuffer*>(m_boMap[m_iboPool[poolName].value(mesh)]);
    }
    return NULL;
}

