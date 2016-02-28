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
template<class T> using vector = std::vector<T, core::cl::gpu_allocator<T>>;

struct SubdivisionTest::Impl
{
    SDL_Window      *mainwindow;
    SDL_GLContext   maincontext;
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

    /* Initialize SDL's Video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    /* Turn on double buffering with a 24bit Z buffer.
    * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Request opengl 3.2 context.
    * SDL doesn't have the ability to choose which profile at this time of writing,
    * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* Create our window centered at 512x512 resolution */
    mainwindow = SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mainwindow) {
        /* Die if creation failed */
        TRACE(error) << "Unable to create window";
        return 1;
    }
    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
    if (!maincontext) {
        TRACE(error) << "ERROR: Unable to create OpenGL context" << std::endl;
        return 1;
    }

    // Now that we have the window created with an apropriate GL context,
    // Setup the engine with O
    SDL_GL_MakeCurrent(mainwindow, maincontext);
    CLManager::instance()->setUseGPU(true);
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL
        | PlastilinaSubsystem::OPENCL
        | PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
    if (!PlastilinaEngine::initialize(flags)) {
        return 1;
    }

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
    TRACE(info) << "vectorPrimitivrTest";
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Edge;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;
    using core::utils::to_string;

    CLManager * oclManager = CLManager::instance();

    ::cl::Context clctx = oclManager->context();
    ::cl::CommandQueue clqueue = oclManager->commandQueue();
    vector<Vertex>::allocator_type gpu_allocator(clctx, clqueue);
    vector<Vertex> list(gpu_allocator);

    Vertex vtx;
    vtx.setIid(1);
    vtx.p = { 0.0f, 0.0f, 1.0f, 1.0f };
    list.push_back(vtx);

    vtx.setIid(2);
    vtx.p = { 0.0f, 1.0f, 1.0f, 1.0f };
    list.push_back(vtx);
    
    vtx.setIid(3);
    vtx.p = { 1.0f, 1.0f, 1.0f, 1.0f };
    list.push_back(vtx);
    
    vtx.setIid(4);
    vtx.p = { 1.0f, 0.0f, 1.0f, 1.0f };
    list.push_back(vtx);

    TRACE(info) << "sizeof(Vertex) = " << sizeof(Vertex);
    TRACE(info) << "List values:";
    unsigned int idx = 0;
    for (const Vertex & v : list)
    {
        TRACE(info) << "addressof(v) = " << std::addressof(v);
        TRACE(info) << "v[" << idx << "].iid=" << v.iid();
        TRACE(info) << "v[" << idx << "].p=" << to_string(v.p);
        idx++;
    }

    vector<Edge> edgeList(gpu_allocator);
    Edge edge;
    edge.setIid(1);
    edge.head = 1;
    edge.pair = 2;
    edge.face = 0;
    edgeList.push_back(edge);
    edge.setIid(2);
    edge.head = 2;
    edge.pair = 1;
    edge.face = 0;
    edgeList.push_back(edge);
    TRACE(info) << "sizeof(Edge) = " << sizeof(Edge);
    TRACE(info) << "List values:";
    idx = 0;
    for (const Edge & v : edgeList)
    {
        TRACE(info) << "addressof(v) = " << std::addressof(v);
        TRACE(info) << "edgeList[" << idx << "].iid=" << v.iid();
        TRACE(info) << "edgeList[" << idx << "].head=" << v.head;
        idx++;
    }
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
        TRACE(debug) << "Vertex: " << vertex.iid();
        if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
            Vertex *vtx = static_cast<Vertex*>(&vertex);
            TRACE(debug) << "  p: " << to_string(vtx->p) ;
        }
    }

    counter = 0;
    auto faceIt = surface->constFaceIterator();
    for (auto & face : faceIt)
    {
        TRACE(debug) << "Face: " << face.iid();
        if (face.type() == (int)FaceHandleType::GPUSUBDIVISION) {
            Face *f = static_cast<Face*>(&face);
            TRACE(debug) << "  FacexId: " << f->iid();
            TRACE(debug) << "  EdgeId: " << surface->edge(f->edge)->iid();
            auto faceVtxIt = f->constVertexIterator(surface.get());
            for (auto & vertex : faceVtxIt)
            {
                TRACE(debug) << "    Vertex: " << vertex.iid();
                if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
                    Vertex *vtx = static_cast<Vertex*>(&vertex);
                    TRACE(debug) << "      p: " << to_string(vtx->p);
                }
            }
        }
    }

    surface->unlock();

    return ret;
}

SubdivisionTest::SubdivisionTest()
    : d_(new Impl())
{
}

SubdivisionTest::~SubdivisionTest()
{
}

int SubdivisionTest::run() {
    TRACEFUNCTION("");
    if (d_->setup() != 0) {
        return -1;
    }
    int err = d_->test();
    d_->cleanup();
    
    return err;
}