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
#include "GpuSubdivision.h"
#include "ClStlAllocator.h"
#include "DocumentModelTest.h"


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
    TRACEFUNCTION();

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
    TRACEFUNCTION();
    vectorPrimitivesTest();
    surfaceTest();

    return 0;
}

int SubdivisionTest::Impl::cleanup() {
    TRACEFUNCTION();
    return 0;
}

int SubdivisionTest::Impl::vectorPrimitivesTest()
{
    TRACE(info) << "vectorPrimitivrTest";
    using core::gpusubdivision::Vertex;
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
    return 0;
}

int SubdivisionTest::Impl::surfaceTest()
{
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;

    doc = std::make_shared<Document>();

    //ISurface * surf = new Box();
    surface = std::make_shared<GpuSubdivision>();
    float hw = 1.0f;
    float hh = 1.0f;
    float hd = 1.0f;

    surface->lock();
    std::vector<size_t> vertexID(8);
    vertexID[0] = surface->addVertex(Point3(-hw, hh, -hd));
    vertexID[1] = surface->addVertex(Point3(hw, hh, -hd));
    vertexID[2] = surface->addVertex(Point3(hw, -hh, -hd));
    vertexID[3] = surface->addVertex(Point3(-hw, -hh, -hd));
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->n = convert_to<cl_float4>(Vector3(-hw, hh, -hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->n = convert_to<cl_float4>(Vector3(hw, hh, -hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->n = convert_to<cl_float4>(Vector3(hw, -hh, -hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->n = convert_to<cl_float4>(Vector3(-hw, -hh, -hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->t = convert_to<cl_float2>(Point2(0, 0));
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->t = convert_to<cl_float2>(Point2(1, 0));
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->t = convert_to<cl_float2>(Point2(1, 1));
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->t = convert_to<cl_float2>(Point2(0, 1));

    vertexID[4] = surface->addVertex(Point3(-hw, hh, hd));
    vertexID[5] = surface->addVertex(Point3(hw, hh, hd));
    vertexID[6] = surface->addVertex(Point3(hw, -hh, hd));
    vertexID[7] = surface->addVertex(Point3(-hw, -hh, hd));
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->n = convert_to<cl_float4>(Vector3(-hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->n = convert_to<cl_float4>(Vector3(hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->n = convert_to<cl_float4>(Vector3(hw, -hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->n = convert_to<cl_float4>(Vector3(-hw, -hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->t = convert_to<cl_float2>(Point2(0, 0));
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->t = convert_to<cl_float2>(Point2(1, 0));
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->t = convert_to<cl_float2>(Point2(1, 1));
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->t = convert_to<cl_float2>(Point2(0, 1));

    std::vector<Vertex::size_t> indexList(4);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[1];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[3];
    surface->addFace(indexList);
    indexList[0] = vertexID[4];
    indexList[1] = vertexID[7];
    indexList[2] = vertexID[6];
    indexList[3] = vertexID[5];
    surface->addFace(indexList);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[7];
    indexList[3] = vertexID[4];
    surface->addFace(indexList);
    indexList[0] = vertexID[5];
    indexList[1] = vertexID[6];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[1];
    surface->addFace(indexList);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[4];
    indexList[2] = vertexID[5];
    indexList[3] = vertexID[1];
    surface->addFace(indexList);
    indexList[0] = vertexID[7];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[6];
    surface->addFace(indexList);

    surface->unlock();

    int counter = 0;
    auto vtxIt = surface->constVertexIterator();
    while (vtxIt.hasNext()) {
        auto vertex = vtxIt.next();
        if (vertex->type() == (int)VertexHandleType::GPUSUBDIVISION) {
            counter++;
            Vertex *vtx = static_cast<Vertex*>(vertex);
            std::cout << "Vertex: " << vtx->p.s[0] << "," << vtx->p.s[1] << ","
                << vtx->p.s[2] << "," << vtx->p.s[3] << "\n";
        }
    }
    std::cout << "Number of vertices: " << counter << std::endl;
    counter = 0;
    auto faceIt = surface->constFaceIterator();
    while (faceIt.hasNext()) {
        auto face = faceIt.next();
        if (face->type() == (int)FaceHandleType::GPUSUBDIVISION) {
            counter++;
            Face *face = static_cast<Face*>(face);

        }
    }
    std::cout << "Number of faces: " << counter << std::endl;
    if (counter == 8) {
        return 0;
    }
    return 1;
}

SubdivisionTest::SubdivisionTest()
    : d_(new Impl())
{
}

SubdivisionTest::~SubdivisionTest()
{
}

int SubdivisionTest::run() {
    TRACEFUNCTION();
    if (d_->setup() != 0) {
        return -1;
    }
    int err = d_->test();
    d_->cleanup();
    
    return err;
}