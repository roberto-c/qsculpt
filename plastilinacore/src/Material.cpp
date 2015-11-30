/***************************************************************************
 *   Copyright (C) 2012 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <atomic>

static std::atomic_int NEXT_ID;

struct Material::Impl {
    int id;
    GlslProgram shaderProgram;
	
	Impl() {
		id = NEXT_ID++;
	}
};

Material::Material() : d_(new Impl)
{
}

Material::~Material()
{
    
}

int Material::iid() const { 
    return d_->id;
}

GlslProgram * Material::shaderProgram() const {
    return &d_->shaderProgram;
}


struct CookTorrance::Impl
{
    VertexShader    vtxShader;
    FragmentShader  fragShader;
};

CookTorrance::CookTorrance()
: Material(),
d(new Impl)
{
    d->vtxShader.loadFromFile(ResourcesManager::resourcesDirectory() + "/Shaders/CookTorrance.vs");
    d->fragShader.loadFromFile(ResourcesManager::resourcesDirectory() + "/Shaders/CookTorrance.fs");
}

CookTorrance::~CookTorrance()
{
    
}

void CookTorrance::load()
{
}

void CookTorrance::unload()
{
    
}

void CookTorrance::setup(const std::shared_ptr<SceneNode> & doc)
{
    
}

void CookTorrance::setup(const std::shared_ptr<const SceneNode> & doc)
{
    
}