/*
*  GpuSubdivision_private.h
*  QSculpt
*
*  Created by Roberto Cabral on 01/05/2016.
*  Copyright 2015 Roberto Cabral. All rights reserved.
*
*/
#pragma once

#include "GpuSubdivision.h"
#include "ClStlAllocator.h"
#include <PlastilinaCore/BufferObject.h>

#include <mutex>

using core::gpusubdivision::Vertex;
using core::gpusubdivision::Edge;
using core::gpusubdivision::Face;
using core::gpusubdivision::Triangle;
template<class T> using vector = std::vector<T, core::cl::gpu_allocator<T>>;

typedef vector<Vertex>  VertexCollection;
typedef vector<Edge>    EdgesCollection;
typedef vector<Face>    FacesCollection;
typedef vector<Triangle> TriangleCollection;

namespace core {
    struct clSurfaceList;

    struct GpuSubdivision::Impl : public IRenderable {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        /** Instance ID of the surface */
        ISurface::size_t _iid;

        GpuSubdivision      *_surface;
        std::unique_ptr<VertexBuffer>  _dataBuffer;
        VertexCollection    *_vertices;
        EdgesCollection     *_edges;
        FacesCollection     *_faces;
        TriangleCollection  *_triangleOutput;

        std::mutex                   _mutex;
        std::unique_lock<std::mutex> _lock;

        Scene*          _scene;
        Color           _color;
        geometry::AABB  _boundingBox;
        bool            _selected;
        int             _callListId;
        bool            _genereateCallList;
        int             _currentResolutionLevel;
        bool            _hasChanged;
        GpuSubdivisionRenderable*   _renderable;
        static bool 				oclInitialized;
        static ::cl::Program  		program;
        static ::cl::Kernel			krnInit;
        static ::cl::Kernel 		krnSubdivideEdges;
        static ::cl::Kernel			krnSubdivideAddFaces;
        static ::cl::Kernel			krnSubdivideAdjustPos;

        Impl(GpuSubdivision * surface);

        ~Impl();

        void 		subdivide(GpuSubdivision * surf);

        static int 	initialize_ocl(void);

        virtual void render(RenderState & state) const;
    };
};
