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
#include <QStandardItem>

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
class SceneNode : public QStandardItem
{
    SceneNode* _parent;
    std::vector<SceneNode*> _children;
    Eigen::Transform3f _transform;

public:
    SceneNode(const QString& = "", SceneNode * = NULL);
    virtual ~SceneNode();
    
    virtual NodeType nodeType() { return NT_Normal; }
    
    const Eigen::Transform3f& transform() const;
    Eigen::Transform3f& transform();
    void setTransform(const Eigen::Transform3f& /*t*/);

    /**
     * Add a node as a child of this node.
     */
    void add(SceneNode *n);
    
    /**
     * Remove node from children list.
     */
    void remove(SceneNode *n);

    /**
     * Function used to render the node on screen.
     */
    virtual void render();
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