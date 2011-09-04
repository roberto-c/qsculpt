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
#ifndef SCENENODE_H
#define SCENENODE_H

#include <vector>
#include "IIterator.h"

class ISurface;

enum NodeType {
    NT_Normal,
    NT_Surface,
    NT_Group
};

/**
 * Basic scene graph node class. 
 *
 * This class inherit from QStandardItem to be able to be displayed
 * in the document tree widget.
 */
class SceneNode 
{
    struct Impl;
    QScopedPointer<Impl> _d;

public:
    SceneNode(const QString& = "", SceneNode * = NULL);
    virtual ~SceneNode();
    
    uint iid() const;
    
    virtual NodeType nodeType() { return NT_Normal; }
    
    /**
     * Sets parent as the new parent of this node. 
     *
     * This node is also added as child in the parent's children node list
     */
    void setParent(SceneNode* parent);
    
    /**
     * Return the parent node of this scene node.
     */
    SceneNode* parent();
    /**
     * Return the parent node of this scene node.
     */
    const SceneNode* parent() const;
    
    /**
     * Return the name of the node.
     */
    std::string name() const;
    
    /**
     * Sets the name of the node. It serves as an identifier.
     */
    void setName(const std::string & name);
    
    /**
     * Returns the affine transformation of this node.
     */
    Eigen::Affine3f   transform() const;
    
    /**
     * Returns the affine transformation of this node as a reference. Useful
     * to chain the transformation with other transformations.
     */
    Eigen::Affine3f & transform();
    
    /**
     * Replace the affine transformation with the new transformation.
     *
     * This is equivalent to
     *
     * SceneNode n();
     * n.transform() = t;
     */
    void setTransform(const Eigen::Affine3f& /*t*/);

    /**
     * Return the cumulative transformation from the root to the parent of this
     * node. It does not include this node transformation.
     */
    Eigen::Affine3f parentTransform() const;
    
    /**
     *
     */
    bool contains(const SceneNode* child) const ;
    
    /**
     * Add a new child node to this scene node
     */
    void add(SceneNode* child);
    
    /**
     * Removes a node from this node. The node to remove must be a direct child
     * from this node.
     */
    void remove(SceneNode* child);
    
    /**
     * Returns the number of children nodes that this node has.
     */
    size_t count() const;
    
    /**
     *
     */
    SceneNode* item(size_t index) const;
    
    /**
     *
     */
    bool isSelected() const;
    
    /**
     *
     */
    void setSelected(bool selected);
    
    /**
     * Function used to render the node on screen.
     */
    virtual void render();
    
    /**
     * Returns an iterator for the children items in this node
     */
    Iterator<SceneNode> iterator();
    
    /**
     * Returns an iterator for the children items in this node
     */
    Iterator<SceneNode> constIterator() const ;
    
private:
    class SceneNodeIterator;
    
    friend class SceneNodeIterator;
};

class SurfaceNode : public SceneNode
{
    ISurface *_surface;

public:
    /**
     *
     */
    SurfaceNode(ISurface *surface, SceneNode * = NULL);
    
    /**
     * Free resources used by this node.
     *
     * The surface is not deleted
     */
    ~SurfaceNode();
    
    virtual NodeType nodeType() { return NT_Surface; }
    
    /**
     * Returns the surface contained in this node.
     */
    ISurface* surface() const;
    
    /**
     * Set the surface contained in this node.
     */
    void setSurface(ISurface *surface);
};

#endif // SCENENODE_H_
