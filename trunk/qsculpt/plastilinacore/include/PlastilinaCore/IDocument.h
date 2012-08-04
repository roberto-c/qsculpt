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
#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/Scene.h>

class ISurface;
class SceneNode;


/**
 * Interface that should implement every kind of document.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class IDocument :public std::enable_shared_from_this<IDocument>
{


public:
    typedef std::shared_ptr<IDocument>   shared_ptr;
    typedef std::shared_ptr<const IDocument>   const_shared_ptr;
    typedef std::weak_ptr<IDocument>     weak_ptr;
    typedef std::weak_ptr<const IDocument>  const_weak_ptr;
    typedef std::unique_ptr<IDocument>   Ptr;
    
    enum ObjectType {
        Mesh,
        Box,
        Sphere,
        Group
    };
    
    enum ChangeType {
        AddObject,
        RemoveObject,
        ObjectChanged
    };
    
    /**
     * IDocument default constructor.
     */
    IDocument(){}

    /**
     * Default virtual destructor.
     */
    virtual ~IDocument(){}
    
    /**
     * Load file
     */
    virtual void loadFile(const std::string& fileName) = 0;
    
    /**
     * Save file
     */
    virtual void saveFile(const std::string& fileName) = 0;

    
    /**
     * This method add a new item into the document.
     *
     * The item is an scene node and it is inserted under the parent stated by
     * the parent QModelIndex. If the index is invalid, then the node is
     * inserted at the root level.
     *
     * @param node Shared pointer to the node to insert. The implementation of
     * this class will share ownership of the node. 
     *
     * @param parent QModelIndex which should be the parent. The node will be
     * inserted as a child of parent node. If parent is invalid, the node is
     * going to be inerted as child of the root node.
     */
//    virtual void addItem(SceneNode::shared_ptr node, 
//                         const QModelIndex & parent = QModelIndex()) = 0;
    
    /**
     * This method searches for a node item with an IID of iid.
     *
     * The item is retained by this Document.
     *
     * @param iid instance id of the item to find
     */
    virtual SceneNode::shared_ptr findItem(uint iid) = 0;
    
//    /**
//     * Return the QModelIndex that corresponds to an SceneNode with ID iid.
//     *
//     * @param iid Instance ID of the node to look for.
//     * @return QModelIndex representing the node.
//     */
//    virtual QModelIndex findItemIndex(uint iid) = 0;
    
    /**
     * 
     */
    virtual void selectObject(int iid) = 0;
    
    /**
     * 
     */
    virtual std::vector<SceneNode::weak_ptr> getSelectedObjects() const = 0;
    
    virtual Iterator<SceneNode> sceneIterator() = 0;
    
    virtual Iterator<SceneNode> constSceneIterator() const = 0;
    
    /**
     * Get the scene object of the document. The scene object
     * is a container with all the objects or actors in the scene.
     */
    virtual Scene::weak_ptr scene() = 0;
    
    /**
     * Get the scene object of the document. The scene object
     * is a container with all the objects or actors in the scene.
     */
    virtual Scene::weak_ptr scene() const = 0;
    
    void changed(IDocument::ChangeType type, ISurface* object);
};

#endif

