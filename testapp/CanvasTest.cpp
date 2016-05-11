/***************************************************************************
*   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
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
#include "stable.h"
#include "CanvasTest.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Canvas.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/Scene.h>
#include <PLastilinaCore/subdivision/GpuSubdivision.h>
#include <PLastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/subdivision/SubdivisionRenderable.h>
#include "DocumentModelTest.h"
#include "PrimitiveFactory.h"

using namespace std;

struct CanvasTest::Impl
{
    CanvasTest                  *test;
    Scene::shared_ptr           scene;
    shared_ptr<PhongMaterial>   material;
    ISurface::shared_ptr        surface;
    Camera::shared_ptr          camera;
    unique_ptr<core::Canvas>    canvas;

    bool                    runUi;

    Impl()
        : test(nullptr)
        , scene(nullptr)
        , runUi(false)
    {}

    int setup();

    int cleanup();
};

int CanvasTest::Impl::setup() {
    TRACEFUNCTION("");
    ResourcesManager rscMgr;

    surface = std::shared_ptr<Subdivision>(core::PrimitiveFactory<Subdivision>::createQuad(1280, 720));
    scene = std::make_shared<Scene>();
    auto surfacenode = std::make_shared<SurfaceNode>(surface.get());
    material = make_shared<PhongMaterial>();
    auto camnode = make_shared<CameraNode>();
    camera = make_shared<Camera>();

    canvas = std::unique_ptr<core::Canvas>(core::Canvas::factory(core::Canvas::OpenGL, 1280, 720));

    if (material && surfacenode && scene && camera && camnode && canvas)
    {
        float aspect_ratio = float(1280) / float(720);
        camnode->setCamera(camera);
        camera->setViewport(0, 0, 1280, 720);
        camera->transform().translate(Vector3(0, 0, -6));
        camera->setOrthoMatrix(0, 1280, 0, 720, -1000, 1000);
        scene->add(surfacenode);
        scene->add(camnode);
        material->load();
        material->setAmbient(Color(0.2f, 0.2f, 0.2f, 1.0f));
        material->setDiffuse(Color(0.2f, 0.2f, 0.8f, 1.0f));
        material->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material->setExponent(200);
        material->setDiffuseTexture(canvas->colorTexture());
        surfacenode->setMaterial(material);

    }
    return 0;
}

int CanvasTest::Impl::cleanup() {
    TRACEFUNCTION("");
    canvas = nullptr;
    if (material) {
        material->unload();
    }
    material = nullptr;
    surface = nullptr;
    scene = nullptr;
    return 0;
}

CanvasTest::CanvasTest()
    : BaseUITest("SubdivisionTest")
    , d_(new Impl())
{
    d_->test = this;
}

CanvasTest::~CanvasTest()
{
}

void CanvasTest::doSetup()
{
    try
    {
        d_->setup();
    }
    catch (core::GlException & e)
    {
        TRACE(error) << "Failed: " << e.error();
    }
    catch (std::exception & e)
    {
        TRACE(error) << "Failed";
    }
}

void CanvasTest::doRun() {
    if (!d_->runUi)
    {
        d_->runUi = true;
    }
}

void CanvasTest::doShutdown()
{
    d_->cleanup();
}

void CanvasTest::resize(int w, int h)
{
    if (d_ && d_->camera)
    {
        float aspect_ratio = float(w) / float(h);
        d_->camera->setViewport(0, 0, w, h);
        d_->camera->setOrthoMatrix(0, 1280, 0, 720, -1000, 1000);
    }
}

void CanvasTest::doRenderFrame()
{
    if (d_->runUi && d_->scene && d_->canvas)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_->canvas->setPenColor(Color(0.3f, 0.4f, 0.6f, 1.0f));
        d_->canvas->begin();
        d_->canvas->drawRectangle(0, 0, 100, 100);
        d_->canvas->end();
        d_->scene->render();
    }
}

void CanvasTest::keyboard(int key, int x, int y)
{
    switch (key) {
    case SDLK_ESCAPE:
        notify(TestEvent::TE_RUN_TEST_END);
        break;
    case SDLK_w:
    case SDLK_UP:
        if (d_->camera) {
            d_->camera->transform().translate(Vector3(0, 0.25f, 0));
        }
        break;
    case SDLK_s:
    case SDLK_DOWN:
        if (d_->camera) {
            d_->camera->transform().translate(Vector3(0, -0.25f, 0));
        }
        break;
    case SDLK_a:
    case SDLK_LEFT:
        if (d_->camera) {
            d_->camera->transform().translate(Vector3(0.25f, 0, 0));
        }
        break;
    case SDLK_d:
    case SDLK_RIGHT:
        if (d_->camera) {
            d_->camera->transform().translate(Vector3(-0.25f, 0, 0));
        }
        break;
    default:
        break;
    }
}