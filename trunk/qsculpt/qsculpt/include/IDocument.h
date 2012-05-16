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

#include <QtCore/QObject>
#include "IIterator.h"
#include "Scene.h"

class ISurface;
class SceneNode;


/**
 * Interface that should implement every kind of document.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class IDocument : public QObject, public std::enable_shared_from_this<IDocument>
{
    Q_OBJECT

public:
    typedef std::shared_ptr<IDocument>   SharedPtr;
    typedef std::shared_ptr<const IDocument>   const_shared_ptr;
    typedef std::weak_ptr<IDocument>     WeakPtr;
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
    virtual void loadFile(const QString& fileName) = 0;
    
    /**
     * Save file
     */
    virtual void saveFile(const QString& fileName) = 0;

    /**
     * 
     */
    virtual void selectObject(int iid) = 0;
    
    /**
     * 
     */
    virtual QList<SceneNode::WeakPtr> getSelectedObjects() const = 0;
    
    virtual Iterator<SceneNode> sceneIterator() = 0;
    
    virtual Iterator<SceneNode> constSceneIterator() const = 0;
    
    /**
     * Get the root scene node of the document.
     */
    virtual SceneNode::WeakPtr rootNode() = 0;
    
    /**
     * Get the root node of the document
     */
    virtual SceneNode::WeakPtr rootNode() const = 0;
    
    /**
     * Get the scene object of the document. The scene object
     * is a container with all the objects or actors in the scene.
     */
    virtual Scene::WeakPtr scene() = 0;
    
    /**
     * Get the scene object of the document. The scene object
     * is a container with all the objects or actors in the scene.
     */
    virtual Scene::WeakPtr scene() const = 0;
    
    
signals:
    void changed(IDocument::ChangeType type, ISurface* object);
};

#endif

