/*
 *  GpuSubdivision_private.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 01/05/2016.
 *  Copyright 2015 Roberto Cabral. All rights reserved.
 *
 */
#pragma once

#include <PlastilinaCore/BufferObject.h>
#include <PlastilinaCore/opencl/ClStlAllocator.h>
#include <PlastilinaCore/subdivision/GpuSubdivision.h>

#include <mutex>
#include <unordered_map>

namespace core
{
namespace gpusubdivision
{
struct GLVertexData
{
    cl_float4 p;
    cl_float4 n;
    cl_float4 c;
    cl_float2 t;
    cl_float2 padding;
};
};
};

using core::gpusubdivision::Vertex;
using core::gpusubdivision::Edge;
using core::gpusubdivision::Face;
using core::gpusubdivision::GLVertexData;
template <class T>
using vector = std::vector<T, core::cl::gpu_allocator<T>>;

typedef vector<Vertex> VertexCollection;
typedef vector<Edge>   EdgesCollection;
typedef vector<Face>   FacesCollection;
typedef std::vector<GLVertexData, core::cl::glgpu_allocator<GLVertexData>>
    GlVertexCollection;

typedef std::pair<Vertex::size_t, Vertex::size_t>       VertexPairKey;
typedef std::unordered_map<VertexPairKey, Edge::size_t> VerticesToEdgeMap;

namespace core
{
struct clSurfaceList;
class GpuSubdivisionRenderable;

struct GpuSubdivision::Impl : public IRenderable
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /** Instance ID of the surface */
    ISurface::size_t _iid;

    GpuSubdivision*               _surface;
    std::unique_ptr<VertexBuffer> _dataBuffer;
    VertexCollection*             _vertices;
    EdgesCollection*              _edges;
    FacesCollection*              _faces;
    GlVertexCollection*           _triangleOutput;

    VerticesToEdgeMap _vertexToEdge;

    std::mutex                   _mutex;
    std::unique_lock<std::mutex> _lock;

    Scene*                    _scene;
    Color                     _color;
    geometry::AABB            _boundingBox;
    bool                      _selected;
    int                       _callListId;
    bool                      _genereateCallList;
    int                       _currentResolutionLevel;
    bool                      _hasChanged;
    GpuSubdivisionRenderable* _renderable;

    Impl(GpuSubdivision* surface);

    ~Impl();

    virtual void render(RenderState& state) const;

    void copyToDevice();
};
};
