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
#ifndef BOMANAGER_H_
#define BOMANAGER_H_

#include <QObject>
#include <QMap>
#include <QHash>
#include <QMultiHash>
#include "BufferObject.h"

class ISurface;

/**
 * Class to manage all vertex buffer objects in the application.
 *
 * Only one manager should exists at any time in the application.
 *
 * TODO: describes the pool part on this class
 */
class BOManager : public QObject
{
	Q_OBJECT

	typedef QHash<GLuint, BufferObject*> IdBufferObjectMap;
	typedef QHash<GLuint, const ISurface*> BOMeshMap;
	typedef QHash<const ISurface*, GLuint> MeshBOMap;
	typedef QHash<QString, MeshBOMap> BOPool;
	
public:
	static BOManager* getInstance();
	
	virtual ~BOManager();
	
	/**
	 * Creates a vertex buffer objects and stablish a relationship between
	 * the buffer object created and an object mesh.
	 *
	 * The same mesh can be binded to differents buffers objects. By doing 
	 * this, the same 3d data can be represented in different ways by different
	 * buffer objects.
	 *
	 * The pointer returned should not be deleted by the caller of the function.
	 * To free the resources of the buffer objects call destroyBO function.
	 *
	 * TODO: describe the notion of pool's on this class.
	 */
	VertexBuffer* createVBO(const QString& poolName, const ISurface* mesh);
	IndexBuffer* createIBO(const QString& poolName, const ISurface* mesh);
	
	/**
	 * Free the buffer object resources.
	 *
	 * This method should be called to free the resources of the buffer objects.
	 * Free the memory used by the buffer objects and delete the relationship of 
	 * the buffer object with the mesh stablished with Create*BO
	 */
	void destroyBO(const QString& poolName, BufferObject* vbo);
	
	/**
	 * Free all the BOs binded to an specific mesh.
	 *
	 * This can be used when a mesh is deleted to unbind and free all the 
	 * memory used by the BOs.
	 */
	void destroyAllMeshBO(const ISurface* mesh);
	
	/**
	 * Free all the BOs under a named pool.
	 *
	 * This can be used when a mesh is deleted to unbind and free all the 
	 * memory used by the BOs.
	 */
	void destroyPool(const QString& poolName);
	
	
	/**
	 * Gets the 3D object binded to the specified buffer object.
	 */
	ISurface* getMesh(const BufferObject* vbo);
	
	/**
	 * Gets the BO associated to the mesh inside an specific BO pool.
	 */
	VertexBuffer* getVBO(const QString& poolName, const ISurface* mesh);
	IndexBuffer* getIBO(const QString& poolName, const ISurface* mesh);
	
public slots:
	void invalidateBO(ISurface* obj);
	
private:
	BOManager();
	
private:
	IdBufferObjectMap m_boMap;
	BOPool m_vboPool;
	BOPool m_iboPool;
	BOMeshMap m_boMeshMap;
	
	static BOManager* m_instance;
};

#endif
