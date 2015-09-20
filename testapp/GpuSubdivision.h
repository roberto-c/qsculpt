/*
 *  GpuSubdivision.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 04/26/2015.
 *  Copyright 2015 Roberto Cabral. All rights reserved.
 *
 */
#pragma once
#ifndef __PLASTILINACORE_CORE_GPUSUBDIVISION_H__
#define __PLASTILINACORE_CORE_GPUSUBDIVISION_H__

#include <vector>
#include <utility>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/opencl/ClUtils.h>

class Mesh;

namespace core
{
namespace gpusubdivision
{
    
    struct Vertex : public VertexHandle {
        typedef Vertex* shared_ptr;
        
        cl_float4 	p;
        cl_float4 	n;
        cl_float4 	c;
        cl_float2   t;
        cl_uint edgeIid;
        cl_uint faceIid;
        
        Vertex()
        : VertexHandle(VertexHandleType::GPUSUBDIVISION)
        , p(), n(), c(), t(), edgeIid(-1), faceIid(-1)
        {}
        Vertex(const Vertex&) = default;
        Vertex(Vertex&&) = default;
    };
    
    struct Edge : public EdgeHandle {
        typedef Edge* shared_ptr;
        
        cl_uint	edgePairIid;
        cl_uint edgeNextIid;
        cl_uint vertexHeadIid;
        cl_uint faceIid;
        
        Edge()
        : EdgeHandle(EdgeHandleType::GPUSUBDIVISION)
        , edgePairIid(-1), edgeNextIid(-1), vertexHeadIid(-1), faceIid(-1)
        {}
    };
    
    struct Face : public FaceHandle {
        typedef Face* shared_ptr;
        
        cl_uint edgeIid;
        
        Face()
        : FaceHandle(FaceHandleType::GPUSUBDIVISION)
        , edgeIid(-1)
        {}
        
        /**
         * Gets an iterator object to traverse all the vertices that form part
         * of this face.
         */
        Iterator<VertexHandle> vertexIterator();

        /**
         * Gets an iterator object to traverse all the vertices that form part
         * of this face.
         */
        Iterator<VertexHandle> constVertexIterator() const;

        /**
         * Gets an iterator object to traverse all the edges that form part
         * of this face.
         */
        Iterator<EdgeHandle> edgeIterator();

        /**
         * Gets an iterator object to traverse all the edges that form part
         * of this face.
         */
        Iterator<EdgeHandle> constEdgeIterator() const;

    };
};
};

namespace core {
    class GpuSubdivision : public ISurface
    {
        struct Impl;
        std::unique_ptr<Impl> _d;
        
    public:
        GpuSubdivision();
        
        /**
         * Construct a subdivision surface from a @class Mesh.
         */
        GpuSubdivision(const Mesh* mesh);
        
        virtual ~GpuSubdivision();
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
        virtual VertexHandle::size_t        addVertex(const Point3& point);
        virtual VertexHandle::size_t        addVertex(VertexHandle* v);
        virtual void                		removeVertex(VertexHandle::size_t id);
        virtual VertexHandle*             	vertex(VertexHandle::size_t index);
        virtual const VertexHandle* 		vertex(VertexHandle::size_t index) const;
        virtual VertexHandle::size_t        numVertices() const;
        virtual EdgeHandle::size_t          addEdge(const EdgeHandle& edge);
        virtual EdgeHandle::size_t          addEdge(VertexHandle::size_t v1, VertexHandle::size_t v2);
        virtual EdgeHandle::size_t 			numEdges() const;
        virtual FaceHandle::size_t          addFace(const std::vector<VertexHandle::size_t>& vertexIndexList);
        virtual void                		replaceFace(size_t index,
                                                        const std::vector<VertexHandle::size_t>& vertexIndexList);
        virtual void                		removeFace( size_t id);
        virtual FaceHandle::size_t          numFaces() const;
        virtual FaceHandle*               	face(FaceHandle::size_t index);
        virtual FaceHandle::size_t                getFaceIndexAtPoint(const Point3& p) const;
        virtual VertexHandle::size_t              getClosestPointAtPoint(const Point3 &p) const;
        virtual std::vector<VertexHandle::size_t> getPointsInRadius(const Point3 &p,
                                                                    float radius) const;
        virtual void                		adjustPointNormal(VertexHandle::size_t index);
        virtual void                		lock() const;
        virtual void                		unlock() const;
        
        virtual std::vector<VertexHandle::size_t> selectedPoints() const;
        virtual void                		setSelectedPoints(const std::vector<VertexHandle::size_t>& indicesArray);
        
        virtual Iterator<VertexHandle>    		vertexIterator();
        virtual Iterator<VertexHandle>    		constVertexIterator() const;
        virtual Iterator<FaceHandle>      		faceIterator();
        virtual Iterator<FaceHandle>      		constFaceIterator() const;
        virtual Iterator<EdgeHandle>      		edgeIterator();
        virtual Iterator<EdgeHandle>      		constEdgeIterator() const;
        
        // End IObject3D interface
        
        /**
         * Returns the Edge Iid that has the vertices V1 and V2
         */
        EdgeHandle::size_t                  edge(VertexHandle::size_t iidVtxTail,
                                                 VertexHandle::size_t iidVtxHead) const;
        
        /**
         * Returns the edge
         */
        EdgeHandle *		                edge(EdgeHandle::size_t iidEdge);
        const EdgeHandle *                	edge(EdgeHandle::size_t iidEdge) const;
        
        //const Object3D& operator=(const Object3D& obj);
        
        void            					addResolutionLevel();
        void            					removeResolutionLevel(int level);
        void            					setWorkingResolutionLevel(int level);
        int             					workingResolutionLevel();
        
        Iterator<VertexHandle> 				vertexIterator(int level);
        Iterator<VertexHandle> 				constVertexIterator(int level) const;
        Iterator<FaceHandle> 				faceIterator(int level);
        Iterator<FaceHandle> 				constFaceIterator(int level) const;
        
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
        EdgeHandle::size_t addEdge(VertexHandle* v1, VertexHandle* v2);
        
        
        // inner classes
    protected:
        class VertexIterator;
        class FaceIterator;
        class EdgeIterator;
        
        friend class VertexIterator;
        friend class FaceIterator;
        friend class EdgeIterator;

    };
};

#endif /* __PLASTILINACORE_CORE_GPUSUBDIVISION_H__ */
