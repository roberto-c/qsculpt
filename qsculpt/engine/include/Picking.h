/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#ifndef IPICKINGRENDERER_H_
#define IPICKINGRENDERER_H_

#include <QVector>
#include <QMap>
#include "BufferObject.h"

class IObject3D;

class PickingRenderer 
{
public:
	typedef QVector<IObject3D*> ObjectList;
	typedef QMap<IObject3D*, QVector<int> > ObjectVerticesMap;
	typedef QMap<IObject3D*, QVector<int> > ObjectFacesMap;
	
	PickingRenderer();
	~PickingRenderer();
	
	ObjectList getSelectedObjects(const ObjectList& list, GLint x, GLint y);
	ObjectVerticesMap getSelectedVertices(const ObjectList& list, GLfloat x, GLfloat y);
	ObjectFacesMap getSelectedFaces(const ObjectList& list, GLfloat x, GLfloat y);
	
private:
	void renderVbo(const IObject3D* mesh, unsigned int objID);
	void renderImmediate(const IObject3D* mesh, unsigned int objID);
	
	VertexBuffer* getVBO(IObject3D* mesh);
	
	void fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo);
	
	
	typedef QMap<IObject3D*, VertexBuffer* > VboContainer;
	
	VboContainer m_vboContainer;
};

#endif
