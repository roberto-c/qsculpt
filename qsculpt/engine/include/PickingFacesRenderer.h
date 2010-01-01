/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
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
#ifndef PICKINGVERTEXRENDERER_H_
#define PICKINGVERTEXRENDERER_H_

#include <QVector>
#include <QMap>
#include "BufferObject.h"
#include "IRenderer.h"

class IObject3D;

class PickingFacesRenderer
{
public:	
	PickingFacesRenderer();
	virtual ~PickingFacesRenderer();
	
	virtual void renderObject(const IObject3D* mesh, GLuint objId);
	
private:
	void renderVbo(const IObject3D* mesh, unsigned int objID);
	void renderImmediate(const IObject3D* mesh, unsigned int objID);
	
	VertexBuffer* getVBO(IObject3D* mesh);
	VertexBuffer* getFlatVBO(IObject3D* mesh);
	
	void fillVertexBuffer(IObject3D* mesh, VertexBuffer* vbo, GLuint objId);
	void fillFlatVertexBuffer(IObject3D* mesh, VertexBuffer* vbo,GLuint objId);
};

#endif
