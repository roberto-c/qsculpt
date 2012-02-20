/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores                      *
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

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <QAtomicInt>

class GlslProgram;


/**
 * Defines the appeareance of the surface or a renderable object.
 *
 * Each material is composed by a shader objects for real time rendring using
 * OpenGL.
 */
class Material
{
    static QAtomicInt NEXT_ID;
    
    int _id;
    
public:
    Material();
    virtual ~Material();
    
    /**
     * Returns the instance ID of the material object.
     */
    int iid() const { return _id; } ;
    
    /**
     * Setup all data to render an object using this material.
     *
     * This function must be implemented by subclasses to bind all necesary 
     * resources, like shaders, textures, lighting parameters, etc.
     */
    virtual void load()=0;
    
    /**
     * Releases all data and resources used by this material.
     */
    virtual void unload() = 0;
    
protected:
    GlslProgram * shaderProgram_;
};

#endif
