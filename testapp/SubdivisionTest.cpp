//
//  SubdivisionTest.cpp
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "stable.h"
#include <iostream>

#include "SubdivisionTest.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

#include "PlastilinaCore/ISurface.h"
#include "PlastilinaCore/Logging.h"
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/subdivision/GpuSubdivision.h>
#include <PlastilinaCore/opencl/ClStlAllocator.h>
#include "DocumentModelTest.h"
#include "PrimitiveFactory.h"

using core::GpuSubdivision;
template<class T> using gpu_vector = std::vector<T, core::cl::gpu_allocator<T>>;
template<class T> using vector = std::vector<T>;

struct SubdivisionTest::Impl
{
    Document::shared_ptr doc;
    ISurface::shared_ptr surface;
    
    int setup();
    
    int test();
    
    int cleanup();

    int vectorPrimitivesTest();

    int surfaceTest();
};

int SubdivisionTest::Impl::setup() {
    TRACEFUNCTION("");

    return 0;
}

int SubdivisionTest::Impl::test() {
    TRACEFUNCTION("");
    vectorPrimitivesTest();
    surfaceTest();

    return 0;
}

int SubdivisionTest::Impl::cleanup() {
    TRACEFUNCTION("");
    return 0;
}

int SubdivisionTest::Impl::vectorPrimitivesTest()
{
    TRACE(info) << "vectorPrimitiveTest";
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Edge;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;
    using core::utils::to_string;

    CLManager * oclManager = CLManager::instance();

    // Reference data
    std::vector<cl_float4> points;
    points.push_back({ 0.0f, 0.0f, 1.0f, 1.0f });
    points.push_back({ 0.0f, 1.0f, 1.0f, 1.0f });
    points.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
    points.push_back({ 1.0f, 0.0f, 1.0f, 1.0f });

    ::cl::Context clctx = oclManager->context();
    ::cl::CommandQueue clqueue = oclManager->commandQueue();
    gpu_vector<Vertex>::allocator_type gpu_allocator(clctx, clqueue);
    gpu_vector<Vertex> list(gpu_allocator);

    Vertex vtx;
    unsigned int idx = 1;

    // lock the container for read/write into the host
    //core::cl::lock_container(list);
    // populate the container with the reference data
    for (auto p : points)
    {
        vtx.setIid(idx);
        vtx.p = p;
        list.push_back(vtx);
        idx++;
    }
    // Verify the data is the same
    bool comp = std::equal(points.begin(), points.end(), list.begin(), 
        [](const cl_float4 & i, const Vertex & j) -> bool {
            return (i.s0 == j.p.s0) && (i.s1 == j.p.s1) 
                && (i.s2 == j.p.s2) && (i.s3 == j.p.s3);
        });
    TRACE(info) << "gpu vector<vertex> host write " << std::string(comp ? "PASSED" : "FAILED");
    core::cl::unlock_container(list);

    // lock the container for read/write. Reads from device into host
    core::cl::lock_container(list);
    // Verify the data is the same
    comp = std::equal(points.begin(), points.end(), list.begin(),
        [](const cl_float4 & i, const Vertex & j) -> bool {
        return (i.s0 == j.p.s0) && (i.s1 == j.p.s1)
            && (i.s2 == j.p.s2) && (i.s3 == j.p.s3);
    });
    TRACE(info) << "gpu vector<vertex> device read " << std::string(comp ? "PASSED" : "FAILED");;

    vector<Edge> edgeHostList;
    gpu_vector<Edge> edgeList(gpu_allocator);
    Edge edge;
    edge.setIid(1);
    edge.head = 1;
    edge.pair = 2;
    edge.face = 0;
    edgeList.push_back(edge);
    edgeHostList.push_back(edge);
    edge.setIid(2);
    edge.head = 2;
    edge.pair = 1;
    edge.face = 0;
    edgeList.push_back(edge);
    edgeHostList.push_back(edge);
    comp = std::equal(edgeList.begin(), edgeList.end(), edgeHostList.begin(),
        [](const Edge & i, const Edge & j) -> bool {
        return (i.iid() == j.iid()) && (i.head == j.head)
            && (i.pair == j.pair) && (i.face == j.face);
    });
    TRACE(info) << "gpu vector<edge> device read " << std::string(comp ? "PASSED" : "FAILED");;

    return 0;
}

int SubdivisionTest::Impl::surfaceTest()
{
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;
    using core::utils::to_string;

    int ret = 1;

    doc = std::make_shared<Document>();

    surface = std::shared_ptr<GpuSubdivision>(core::PrimitiveFactory<GpuSubdivision>::createBox());
    
    surface->lock();
    int counter=0;
    auto vtxIt = surface->constVertexIterator();
    for (auto & vertex : vtxIt)
    {
        TRACE(trace) << "Vertex: " << vertex.iid();
        if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
            Vertex *vtx = static_cast<Vertex*>(&vertex);
            TRACE(trace) << "  p: " << to_string(vtx->p) ;
        }
    }

    counter = 0;
    auto faceIt = surface->constFaceIterator();
    for (auto & face : faceIt)
    {
        TRACE(trace) << "Face: " << face.iid();
        if (face.type() == (int)FaceHandleType::GPUSUBDIVISION) {
            Face *f = static_cast<Face*>(&face);
            TRACE(trace) << "  FacexId: " << f->iid();
            TRACE(trace) << "  EdgeId: " << surface->edge(f->edge)->iid();
            auto faceVtxIt = f->constVertexIterator(surface.get());
            for (auto & vertex : faceVtxIt)
            {
                TRACE(trace) << "    Vertex: " << vertex.iid();
                if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
                    Vertex *vtx = static_cast<Vertex*>(&vertex);
                    TRACE(trace) << "      p: " << to_string(vtx->p);
                }
            }
        }
    }

    surface->unlock();

    return ret;
}

SubdivisionTest::SubdivisionTest()
    : BaseTest("SubdivisionTest")
    , d_(new Impl())
{
}

SubdivisionTest::~SubdivisionTest()
{
}

void SubdivisionTest::setup()
{
    d_->setup();
}

void SubdivisionTest::run() {
    int err = d_->test();
}

void SubdivisionTest::shutdown()
{
    d_->cleanup();
}

