/*
 *  Subdivision.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <vector>
#include <utility>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Aabb.h>

class Vertex;
class Edge;
class SubdivisionScheme;
class Mesh;

/**
 * Class that implement subdivision surfaces.
 */
class Subdivision : public ISurface
{
    struct Impl;
    std::unique_ptr<Impl> _d;

public:
    Subdivision();
    
    /**
     * Construct a subdivision surface from a @class Mesh. 
     */
    Subdivision(const Mesh* mesh);
    
    virtual ~Subdivision();
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

    // End IObject3D interface
		
    /**
     * Returns the Edge Iid that has the vertices V1 and V2
     */
	Edge::size_t                      	edge(Vertex::size_t iidVtxTail,
									 		Vertex::size_t iidVtxHead) const;
	
	/**
	 * Returns the edge
	 */
	Edge *		                		edge(Edge::size_t iidEdge);
	const Edge *                		edge(Edge::size_t iidEdge) const;

    //const Object3D& operator=(const Object3D& obj);

    void            					addResolutionLevel();
    void            					removeResolutionLevel(int level);
    void            					setWorkingResolutionLevel(int level);
    int             					workingResolutionLevel();
    
    Iterator<Vertex> 					vertexIterator(int level);
    Iterator<Vertex> 					constVertexIterator(int level) const;
    Iterator<Face> 						faceIterator(int level);
    Iterator<Face> 						constFaceIterator(int level) const;

    /**
     * Debugging function used to print the memory usage of this
     * object.
     */
    void printMemoryInfo() const;
    
    /**
     * Convert subdivision surface to a regular Mesh.
     *
     * @param level level of the surface to convert to mesh.
     */
    Mesh* convertToMesh(int level = -1);
    
    /**
     * Uses mesh as base mesh for this subdivision surface.
     *
     * @param mesh mesh to convert from.
     */
    void convertFromMesh(const Mesh* mesh);
    
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();

    /**
      *
      */
	Edge::size_t addEdge(Vertex* v1, Vertex* v2);


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
