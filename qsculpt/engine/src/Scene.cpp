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
#include "StdAfx.h"
#include "Scene.h"


namespace {
    class SceneNodeIterator : public IIterator<SceneNode>
    {
        SceneNode* _parent;
        
    public:
        SceneNodeIterator(SceneNode* parent) ;
        
        /**
         * Function to copy iterators of the same type.
         */
        virtual IIterator<SceneNode>* clone() const;
        
        /**
         * Return true if the iterator has more elements (i.e. it is not at the
         * end)
         */
        virtual bool hasNext() const;
        
        /**
         * Returns true if the iterator is not at the beginning of the iteration
         */
        virtual bool hasPrevious() const;
        
        /**
         * Returns the next element and advance the iterator by one.
         */
        virtual SceneNode & next();
        
        /**
         * Returns the next element and advance the iterator by one.
         */
        virtual const SceneNode & next() const;
        
        /**
         * Returns the previous elements and move the iterator one position
         * backwards.
         */
        virtual SceneNode & previous();
        
        /**
         * Returns the previous elements and move the iterator one position
         * backwards.
         */
        virtual const SceneNode & previous() const;
        
        /**
         * Set the current position to pos relative to origin.
         *
         * @param pos number of elements to jump relative to origin
         * @param origin states the reference to jump.
         */
        virtual bool seek(int pos, IteratorOrigin origin) const;
    };
    
    SceneNodeIterator::SceneNodeIterator(SceneNode* parent)
    : _parent(parent)
    {
    }
    
    IIterator<SceneNode>* SceneNodeIterator::clone() const
    {
        return new SceneNodeIterator(_parent);
    }
    
    bool SceneNodeIterator::hasNext() const
    {
        return false;
    }
    
    bool SceneNodeIterator::hasPrevious() const
    {
        return false;
    }
    
    SceneNode & SceneNodeIterator::next()
    {
        throw std::runtime_error("Not implemented");
    }
    
    const SceneNode & SceneNodeIterator::next() const
    {
        throw std::runtime_error("Not implemented");
    }
    
    SceneNode & SceneNodeIterator::previous()
    {
        throw std::runtime_error("Not implemented");
    }
    
    const SceneNode & SceneNodeIterator::previous() const
    {
        throw std::runtime_error("Not implemented");
    }
    
    bool SceneNodeIterator::seek(int pos, IteratorOrigin origin) const
    {
        throw std::runtime_error("Not implemented");
    }
};

Scene::Scene()
{
}

Scene::Scene(const std::string& /*name*/)
{
}


Scene::~Scene()
{
}

SceneNode* Scene::findByName(const QString& name)
{
    return NULL;
}

SceneNode* Scene::findByIID(uint IID)
{
    return NULL;
}


Iterator<SceneNode> Scene::iterator()
{
    return Iterator<SceneNode>(new SceneNodeIterator(NULL));
}

Iterator<SceneNode> Scene::constIterator() const
{
    return Iterator<SceneNode>(new SceneNodeIterator(NULL));
}
