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
#include "StdAfx.h"
#include "SceneNode.h"

#include <algorithm>

SceneNode::SceneNode(const QString& name, SceneNode *parent)
: QStandardItem()
{
    this->setText(name);
    if (parent)
    {
        parent->appendRow(this);
    }
}

SceneNode::~SceneNode()
{
}

const Eigen::Transform3f& SceneNode::transform() const
{ 
    return _transform; 
}

Eigen::Transform3f& SceneNode::transform() 
{ 
    return _transform; 
}

void SceneNode::setTransform(const Eigen::Transform3f& /*t*/)
{
}


void SceneNode::add(SceneNode *n)
{
    _children.push_back(n);
}

void SceneNode::remove(SceneNode *n)
{
    std::vector<SceneNode*>::iterator it;
    it = find(_children.begin(), _children.end(), n);
    if (it != _children.end())
        _children.erase(it);
}

void SceneNode::render()
{
}



SurfaceNode::SurfaceNode(ISurface *surface, SceneNode *parent)
: SceneNode("", parent)
, _surface(surface)
{
}

SurfaceNode::~SurfaceNode()
{
}

ISurface* SurfaceNode::surface() const
{
    return _surface;
}

void SurfaceNode::setSurface(ISurface *surface)
{
    _surface = surface;
}
