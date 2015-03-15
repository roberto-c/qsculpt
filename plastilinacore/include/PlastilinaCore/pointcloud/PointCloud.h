//
//  PointCloud.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//
//
#pragma once
#ifndef PlastilinaCore_PointCloud_h
#define PlastilinaCore_PointCloud_h

#include <PlastilinaCore/ISurface.h>

class PointCloud : public ISurface
{
    
public:
    typedef std::shared_ptr<PointCloud>   shared_ptr;
    typedef std::weak_ptr<PointCloud>     weak_ptr;
    typedef std::unique_ptr<PointCloud>   Ptr;
    
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    PointCloud();
    
    virtual ~PointCloud();
    
    /******************************************************************************
     * IObject3D interface
     */
    virtual size_t              		iid() const;
    virtual bool                		hasChanged();
    virtual void                		setChanged(bool val);
    virtual void                		setScene(Scene* scene) ;
    virtual Scene*              		scene() const;
	virtual const IRenderable* 			renderable() const;
    virtual const geometry::AABB& 		boundingBox() const;
    virtual void                		setColor(const Color& color);
    virtual Color               		color() const;
    virtual void                		setSelected(bool val);
    virtual bool                		isSelected() const;
    virtual Vertex::size_t              addVertex(const Point3& point);
    virtual Vertex::size_t              addVertex(Vertex* v);
    virtual void                		removeVertex(Vertex::size_t id);
    virtual Vertex*             		vertex(Vertex::size_t index);
    virtual const Vertex* 				vertex(Vertex::size_t index) const;
    virtual Vertex::size_t              numVertices() const;
    virtual Edge::size_t              	addEdge(const Edge& edge);
    virtual Edge::size_t              	addEdge(Vertex::size_t v1, Vertex::size_t v2);
	virtual Edge::size_t 				numEdges() const;
    virtual Face::size_t              	addFace(const std::vector<Vertex::size_t>& vertexIndexList);
    virtual void                		replaceFace(size_t index,
                                                    const std::vector<Vertex::size_t>& vertexIndexList);
    virtual void                		removeFace( size_t id);
    virtual Face::size_t              	numFaces() const;
    virtual Face*               		face(Face::size_t index);
    virtual Face::size_t              	getFaceIndexAtPoint(const Point3& p) const;
    virtual Vertex::size_t              getClosestPointAtPoint(const Point3 &p) const;
    virtual std::vector<Vertex::size_t> getPointsInRadius(const Point3 &p,
                                                          float radius) const;
    virtual void                		adjustPointNormal(Vertex::size_t index);
    virtual void                		lock() const;
    virtual void                		unlock() const;
    
    virtual std::vector<Vertex::size_t> selectedPoints() const;
    virtual void                		setSelectedPoints(const std::vector<Vertex::size_t>& indicesArray);
    
    virtual Iterator<Vertex>    		vertexIterator();
    virtual Iterator<Vertex>    		constVertexIterator() const;
    virtual Iterator<Face>      		faceIterator();
    virtual Iterator<Face>      		constFaceIterator() const;
    virtual Iterator<Edge>      		edgeIterator();
    virtual Iterator<Edge>      		constEdgeIterator() const;

    
    /**
     * Used to notify mesh has changed.
     */
    void meshChanged(ISurface* mesh);
    
private:
    struct Impl;
    std::unique_ptr<Impl>	d;
    
    // inner classes
protected:
    class VertexIterator;
    class FaceIterator;
    class EdgeIterator;
    
    friend class VertexIterator;
    friend class FaceIterator;
    friend class EdgeIterator;
};

#endif
