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
#include <memory>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Vector.h>

class ISurface;
class Material;
class Camera;

enum NodeType {
    NT_Normal,
    NT_Surface,
    NT_Group,
    NT_Light,
    NT_Camera
};

/**
 * Basic scene graph node class. 
 *
 * This class inherit from QStandardItem to be able to be displayed
 * in the document tree widget.
 */
class DLLEXPORT SceneNode : public std::enable_shared_from_this<SceneNode>
{
    struct Impl;
    std::unique_ptr<Impl> _d;

public:
    typedef std::shared_ptr<SceneNode>          shared_ptr;
    typedef std::shared_ptr<const SceneNode>    const_shared_ptr;
    typedef std::weak_ptr<SceneNode>            weak_ptr;
    typedef std::weak_ptr<const SceneNode>      const_weak_ptr;
    typedef std::unique_ptr<SceneNode>          ptr;
    typedef std::unique_ptr<const SceneNode>    const_ptr;
    
    SceneNode(const std::string& = "");
    
    virtual ~SceneNode();
    
    uint32_t iid() const;
    
    virtual NodeType nodeType() const { return NT_Normal; }
    
    /**
     * Sets parent as the new parent of this node. 
     *
     * This node is also added as child in the parent's children node list
     */
    void setParent(weak_ptr parent);
    
    /**
     * Return the parent node of this scene node.
     */
    weak_ptr parent();
    /**
     * Return the parent node of this scene node.
     */
    const weak_ptr parent() const;
    
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
     * Map world coordinates to local coordinates.
     *
     * It chains the parent node transformation and this node transformation
     * to compute the local coordinates.
     */
    Point3 worldToLocal(Point3 p) const;
    
    /**
     * Map local coordinates to world coordinates.
     *
     * It chains the parent node transformation and this node transformation
     * to compute the local coordinates.
     */
    Point3 localToWorld(Point3 p) const;
    
    /**
     *
     */
    bool contains(const weak_ptr child) const ;
    
    /**
     * Add a new child node to this scene node
     */
    void add(weak_ptr child);
    
    /**
     * Removes a node from this node. The node to remove must be a direct child
     * from this node.
     */
    void remove(weak_ptr child);
    
    /**
     * Removed the node with an instance id of iid.
     */
    void remove(uint32_t iid);
    
    /**
     * Returns the number of children nodes that this node has.
     */
    size_t count() const;
    
    /**
     * Returns the item at position index.
     */
    weak_ptr item(size_t index) const;
    
    /**
     * This function returns the item index of a child of this node.
     *
     * @return true if node found, otherwise, false.
     */
    bool itemIndex(const weak_ptr child, size_t * index) const; 
    
    /**
     * This function returns the item index of a child of this node with the
     * ID iid.
     *
     * @return true if node found, otherwise, false.
     */
    bool itemIndexFromIid(uint32_t childIID, size_t * index) const; 
    
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
    virtual void render(const RenderState * state) const;
    
    /**
     * Returns an iterator for the children items in this node
     */
    Iterator<SceneNode> iterator();
    
    /**
     * Returns an iterator for the children items in this node
     */
    Iterator<SceneNode> constIterator() const ;
    
    /**
     * Returns an iterators to traverse all tree of the scene.
     */
    Iterator<SceneNode> treeIterator();
    
    /**
     * Returns an iterators to traverse all tree of the scene.
     */
    Iterator<SceneNode> treeIterator() const;
    
    /**
     *
     */
    virtual void dump();
    
protected:
    SceneNode(const std::string & name, NodeType nodetype);
    
private:
    // disabled copy semantics for now
    SceneNode(const SceneNode &);
    SceneNode & operator=(const SceneNode &);
    
    class SceneNodeIterator;
    class SceneNodeTreeIterator;
    
    friend class SceneNodeIterator;
    friend class SceneNodeTreeIterator;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class DLLEXPORT SurfaceNode : public SceneNode, public IRenderable
{
    ISurface *surface_;
    std::shared_ptr<Material> material_;

public:
    typedef std::shared_ptr<SurfaceNode>        shared_ptr;
    typedef std::weak_ptr<SurfaceNode>    		weak_ptr;
    typedef std::shared_ptr<const SurfaceNode>  const_shared_ptr;
    typedef std::weak_ptr<const SurfaceNode>    const_weak_ptr;
    typedef std::unique_ptr<SurfaceNode>        ptr;
    
    /**
     *
     */
    SurfaceNode(ISurface *surface = nullptr, const std::string & name = "NoName");
    
    //SurfaceNode(const ISurface *surface = NULL);
    
    /**
     * Free resources used by this node.
     *
     * The surface is not deleted
     */
    virtual ~SurfaceNode();
    
    virtual NodeType nodeType() const { return NT_Surface; }
    
    /**
     * Returns the surface contained in this node.
     */
    ISurface* surface() const;
    
    /**
     * Returns the surface contained in this node.
     */
    ISurface* surface();
    
    /**
     * Set the surface contained in this node.
     */
    void setSurface(ISurface *surface);
    
    /**
     * Returns the material used for this surface
     */
    std::shared_ptr<Material> material() const;
    
    /**
     * Set the material used for this surface.
     */
    void setMaterial(const std::shared_ptr<Material> & material);
	
	virtual void render(RenderState & state) const;
    
private:
    // disabled copy semantics for now
    SurfaceNode(const SurfaceNode &);
    SurfaceNode & operator=(const SurfaceNode &); 
};

class DLLEXPORT LightNode : public SceneNode
{
public:
    LightNode(const std::string & name = "Light");
    
    virtual ~LightNode();
    
    void setColor(const Eigen::Vector4f & color);
    Eigen::Vector4f color() const;
};

/**
 * SceneNode that contains a camera
 *
 *
 */
class DLLEXPORT CameraNode : public SceneNode
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    typedef std::shared_ptr<CameraNode>       	shared_ptr;
    typedef std::weak_ptr<CameraNode>    		weak_ptr;
    typedef std::shared_ptr<const CameraNode> 	const_shared_ptr;
    typedef std::weak_ptr<const CameraNode>		const_weak_ptr;
    
    CameraNode(const std::shared_ptr<Camera> & cam = std::shared_ptr<Camera>(),
               const std::string & name = "Camera");
    
    virtual ~CameraNode();
    
    /**
     * Set the camera object this node refers to
     */
    void setCamera(const std::shared_ptr<Camera> & cam);
    
    /**
     * Gets a pointer to the camera object this node refers to
     */
    std::shared_ptr<Camera> camera() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

/**
 * Effect node.
 *
 * This node adds an effect to the the children objects.
 *
 * Effects are program shaders that are made active before rendering children.
 */
class DLLEXPORT EffectNode: public SceneNode
{
public:
    typedef std::shared_ptr<EffectNode>       	shared_ptr;
    typedef std::weak_ptr<EffectNode>    		weak_ptr;
    typedef std::shared_ptr<const EffectNode> 	const_shared_ptr;
    typedef std::weak_ptr<const EffectNode>		const_weak_ptr;
    
    EffectNode(const std::shared_ptr<Material> & material = nullptr,
               const std::string & name = "effect");
    
    virtual ~EffectNode();
    
    /**
     * Function to get the material that implement the effect.
     */
    std::shared_ptr<Material> material() const;
    
    /**
     * Function to set the material that implements the effect.
     */
    void setMaterial(const std::shared_ptr<Material> & material);
    
};
#endif // SCENENODE_H_
