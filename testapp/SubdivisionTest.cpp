//
//  SubdivisionTest.cpp
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "Stable.h"
#include <iostream>

#include "SubdivisionTest.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>

#include <PLastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/opencl/ClStlAllocator.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/subdivision/GpuSubdivision.h>
#include <PlastilinaCore/subdivision/GpuSubdivisionRenderable.h>
#include <PlastilinaCore/subdivision/SubdivisionRenderable.h>
#include "DocumentModelTest.h"
#include "PlastilinaCore/ISurface.h"
#include "PlastilinaCore/Logging.h"
#include "PrimitiveFactory.h"

using namespace std;
using core::GpuSubdivision;
template <class T>
using gpu_vector = std::vector<T, core::cl::gpu_allocator<T>>;

struct SubdivisionTest::Impl
{
    SubdivisionTest*          test;
    Scene::shared_ptr         scene;
    shared_ptr<PhongMaterial> material;
    ISurface::shared_ptr      surface;
    Camera::shared_ptr        camera;
    SDL_Surface*              texture;
    gl::Texture2D::shared_ptr glTexture1;
    gl::Texture2D::shared_ptr glTexture2;

    bool runUi;

    Impl()
        : test(nullptr)
        , scene(nullptr)
        , surface(nullptr)
        , texture(nullptr)
        , runUi(false)
    {
    }

    int setup();

    int cleanup();

    int vectorPrimitivesTest();

    int surfaceTest();
};

int SubdivisionTest::Impl::setup()
{
    TRACEFUNCTION("");
    ResourcesManager rscMgr;

    // surface =
    // std::shared_ptr<GpuSubdivision>(core::PrimitiveFactory<GpuSubdivision>::createQuad(1280,
    // 720));
    surface = std::shared_ptr<GpuSubdivision>(
        core::PrimitiveFactory<GpuSubdivision>::createQuad(2, 2));
    // surface =
    // std::shared_ptr<Subdivision>(core::PrimitiveFactory<Subdivision>::createBox());
    scene            = std::make_shared<Scene>();
    auto surfacenode = std::make_shared<SurfaceNode>(surface.get());
    material         = make_shared<PhongMaterial>();
    auto camnode     = make_shared<CameraNode>();
    camera           = make_shared<Camera>();

    std::string texturePath =
        rscMgr.findResourcePath("Texture_1280x720", "png");
    texture = IMG_Load(texturePath.c_str());
    if (!texture)
    {
        TRACE(debug) << "Failed to load texture";
        TRACE(debug) << "SDL Error: " << SDL_GetError();
        throw std::runtime_error("Failed to load asset: Texture01.png");
    }
    else
    {
        TRACE(debug) << "Texture loaded";
        TRACE(debug) << "TextureFormat: "
                     << SDL_GetPixelFormatName(texture->format->format);
        TRACE(debug) << "Size " << texture->w << "x" << texture->h;
        TRACE(debug) << "Bytes Per Pixel: "
                     << int(texture->format->BytesPerPixel);

        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        glTexture1 = std::make_shared<gl::Texture2D>();
        glTexture1->bind();
        glTexture1->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        glTexture1->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
        glTexture1->texImage2D(0, GL_RGBA8, texture->w, texture->h, 0,
                               GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
        glTexture2 = std::make_shared<gl::Texture2D>();
        glTexture2->bind();
        glTexture2->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        glTexture2->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
        glTexture2->texImage2D(0, GL_RGBA8, texture->w, texture->h, 0,
                               GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
    }
    if (material && surfacenode && scene && camera && camnode)
    {
        float aspect_ratio = float(1280) / float(720);
        camnode->setCamera(camera);
        camera->setViewport(0, 0, 1280, 720);
        camera->transform()
            .translate(Vector3(0, 2, -6))
            .rotate(Eigen::Quaternionf(M_PI_4, 0, 0, 1));
        camera->setOrthoMatrix(-10, 10, -10 / aspect_ratio, 10 / aspect_ratio,
                               -1000, 1000);
        camera->setPerspectiveMatrix(45, aspect_ratio, 1, 10);
        // camera->setOrthoMatrix(0, 1280, 0, 720, -1000, 1000);
        scene->add(surfacenode);
        scene->add(camnode);
        material->load();
        material->setAmbient(Color(0.2f, 0.2f, 0.2f, 1.0f));
        material->setDiffuse(Color(0.2f, 0.2f, 0.8f, 1.0f));
        material->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material->setExponent(200);
        material->setDiffuseTexture(glTexture1);
        surfacenode->setMaterial(material);
    }
    return 0;
}

int SubdivisionTest::Impl::cleanup()
{
    TRACEFUNCTION("");
    material->unload();
    SDL_FreeSurface(texture);
    glTexture1 = nullptr;
    material   = nullptr;
    surface    = nullptr;
    scene      = nullptr;
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

    CLManager* oclManager = CLManager::instance();

    // Reference data
    std::vector<cl_float4> points;
    points.push_back({0.0f, 0.0f, 1.0f, 1.0f});
    points.push_back({0.0f, 1.0f, 1.0f, 1.0f});
    points.push_back({1.0f, 1.0f, 1.0f, 1.0f});
    points.push_back({1.0f, 0.0f, 1.0f, 1.0f});

    ::cl::Context                      clctx   = oclManager->context();
    ::cl::CommandQueue                 clqueue = oclManager->commandQueue();
    gpu_vector<Vertex>::allocator_type gpu_allocator(clctx, clqueue);
    gpu_vector<Vertex>                 list(gpu_allocator);

    Vertex       vtx;
    unsigned int idx = 1;

    // lock the container for read/write into the host
    // core::cl::lock_container(list);
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
                           [](const cl_float4& i, const Vertex& j) -> bool {
                               return (i.s0 == j.p.s0) && (i.s1 == j.p.s1) &&
                                      (i.s2 == j.p.s2) && (i.s3 == j.p.s3);
                           });
    TRACE(info) << "gpu vector<vertex> host write "
                << std::string(comp ? "PASSED" : "FAILED");
    core::cl::unlock_container(list);

    // lock the container for read/write. Reads from device into host
    core::cl::lock_container(list);
    // Verify the data is the same
    comp = std::equal(points.begin(), points.end(), list.begin(),
                      [](const cl_float4& i, const Vertex& j) -> bool {
                          return (i.s0 == j.p.s0) && (i.s1 == j.p.s1) &&
                                 (i.s2 == j.p.s2) && (i.s3 == j.p.s3);
                      });
    TRACE(info) << "gpu vector<vertex> device read "
                << std::string(comp ? "PASSED" : "FAILED");
    ;

    vector<Edge>     edgeHostList;
    gpu_vector<Edge> edgeList(gpu_allocator);
    Edge             edge;
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
                      [](const Edge& i, const Edge& j) -> bool {
                          return (i.iid() == j.iid()) && (i.head == j.head) &&
                                 (i.pair == j.pair) && (i.face == j.face);
                      });
    TRACE(info) << "gpu vector<edge> device read "
                << std::string(comp ? "PASSED" : "FAILED");
    ;

    return 0;
}

int SubdivisionTest::Impl::surfaceTest()
{
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;
    using core::utils::to_string;

    int ret = 1;

    surface->lock();
    int  counter = 0;
    auto vtxIt   = surface->constVertexIterator();
    for (auto& vertex : vtxIt)
    {
        TRACE(trace) << "Vertex: " << vertex.iid();
        if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION)
        {
            Vertex* vtx = static_cast<Vertex*>(&vertex);
            TRACE(trace) << "  p: " << to_string(vtx->p);
        }
    }

    counter     = 0;
    auto faceIt = surface->constFaceIterator();
    for (auto& face : faceIt)
    {
        TRACE(trace) << "Face: " << face.iid();
        if (face.type() == (int)FaceHandleType::GPUSUBDIVISION)
        {
            Face* f = static_cast<Face*>(&face);
            TRACE(trace) << "  FacexId: " << f->iid();
            TRACE(trace) << "  EdgeId: " << surface->edge(f->edge)->iid();
            auto faceVtxIt = f->constVertexIterator(surface.get());
            for (auto& vertex : faceVtxIt)
            {
                TRACE(trace) << "    Vertex: " << vertex.iid();
                if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION)
                {
                    Vertex* vtx = static_cast<Vertex*>(&vertex);
                    TRACE(trace) << "      p: " << to_string(vtx->p);
                }
            }
        }
    }

    surface->unlock();

    return ret;
}

SubdivisionTest::SubdivisionTest()
    : BaseUITest("SubdivisionTest")
    , d_(new Impl())
{
    d_->test = this;
}

SubdivisionTest::~SubdivisionTest() {}

void SubdivisionTest::doSetup() { d_->setup(); }

void SubdivisionTest::doRun()
{
    if (!d_->runUi)
    {
        d_->vectorPrimitivesTest();
        d_->surfaceTest();
        d_->runUi = true;
    }
}

void SubdivisionTest::doShutdown() { d_->cleanup(); }

void SubdivisionTest::resize(int w, int h)
{
    if (d_ && d_->camera)
    {
        float aspect_ratio = float(w) / float(h);
        d_->camera->setViewport(0, 0, w / 2, h);
        d_->camera->setOrthoMatrix(-10, 10, -10 * aspect_ratio,
                                   10 * aspect_ratio, -1000, 1000);
    }
}

void SubdivisionTest::doRenderFrame()
{
    if (d_->runUi && d_->scene)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        int   w = 1280, h = 720;
        float aspect_ratio = float(w / 2) / float(h);
        d_->camera->setViewport(0, 0, w / 2, h);
        glViewport(0, 0, w / 2, h);
        // d_->camera->setOrthoMatrix(-10, 10, -10 * aspect_ratio, 10 *
        // aspect_ratio, -1000, 1000);
        d_->scene->render();
        d_->camera->setViewport(w / 2, 0, w / 2, h);
        glViewport(w / 2, 0, w / 2, h);
        // d_->camera->setOrthoMatrix(-10, 10, -10 * aspect_ratio, 10 *
        // aspect_ratio, -1000, 1000);
        d_->scene->render();
    }
}

void SubdivisionTest::keyboard(int key, int x, int y)
{
    switch (key)
    {
    case SDLK_ESCAPE:
        notify(TestEvent::TE_RUN_TEST_END);
        break;
    case SDLK_w:
    case SDLK_UP:
        if (d_->camera)
        {
            d_->camera->transform().translate(Vector3(0, 0.25f, 0));
        }
        break;
    case SDLK_s:
    case SDLK_DOWN:
        if (d_->camera)
        {
            d_->camera->transform().translate(Vector3(0, -0.25f, 0));
        }
        break;
    case SDLK_a:
    case SDLK_LEFT:
        if (d_->camera)
        {
            d_->camera->transform().translate(Vector3(0.25f, 0, 0));
        }
        break;
    case SDLK_d:
    case SDLK_RIGHT:
        if (d_->camera)
        {
            d_->camera->transform().translate(Vector3(-0.25f, 0, 0));
        }
        break;
    default:
        break;
    }
}

void SubdivisionTest::mouseClick(uint32_t button, uint32_t state, int x,
                                 int y)
{
}

void SubdivisionTest::mouseMove(uint32_t state, int x, int y) {}