//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "stable.h"
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Logging.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

#ifdef __APPLE__
#include <mach-o/dyld.h> // for application directory
#endif

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
//#include <omp.h>


#include "TestApp.h"

#include "PlastilinaCore/Color.h"
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Camera.h>
#include "PlastilinaCore/HEdge.h"
#include "PlastilinaCore/Face.h"
#include "PlastilinaCore/Point3D.h"
#include "PlastilinaCore/Octree.h"
#include "PlastilinaCore/Vector.h"
#include "PlastilinaCore/Scene.h"
#include "PlastilinaCore/SceneNode.h"
#include "PlastilinaCore/geometry/Sphere.h"
#include "PlastilinaCore/geometry/Ray.h"
#include "PlastilinaCore/FlatRenderer.h"
#include "PlastilinaCore/SmoothRenderer.h"
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/material/PointMaterial.h>
#include <PlastilinaCore/PointRenderer.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/ClStlAllocator.h>
#include "PlastilinaCore/opengl/GlslShader.h"
#include "PlastilinaCore/opengl/GlslProgram.h"
#include <PlastilinaCore/opengl/Texture.h>
#include <PlastilinaCore/physics/SimSystem.h>
#include <PlastilinaCore/physics/Actor.h>
#include <PlastilinaCore/physics/ForceFunctors.h>
#include <PlastilinaCore/pointcloud/PointCloud.h>
#include <PlastilinaCore/ResourcesManager.h>
#include "PlastilinaCore/subdivision/Sphere.h"
#include "PlastilinaCore/subdivision/Box.h"
#include "PlastilinaCore/subdivision/Subdivision.h"
#include "PlastilinaCore/subdivision/GpuSubdivision.h"
#include <PlastilinaCore/Utilities.h>
#include "PlastilinaCore/Vertex.h"

#include "Subdivision.h"
#include "ParticleSystem.h"
#include "CLRender.h"
#include "TestMaterial.h"
#include "PrimitiveFactory.h"
#include "SubdivisionTest.h"

namespace po = boost::program_options;

struct TestApp::Impl {
    po::options_description optionsDesc;
    po::variables_map   options;
    bool                running;
    bool                initialized;
    SDL_Surface*        surfDisplay;
    SDL_Window*         mainwindow; /* Our window handle */
    SDL_GLContext       maincontext; /* Our opengl context handle */
	SDL_Surface         *texture;
    
    std::string         appName;
    Scene::shared_ptr   scene;
    SceneNode::shared_ptr object;
    
    GlslProgram     *glslProgram;
    VertexShader    *vtxShader;
    FragmentShader  *fragShader;
    
    std::shared_ptr<PointMaterial>      material;
    std::shared_ptr<TestMaterial>       material2;
    std::shared_ptr<PhongMaterial>      material3;
	
	CLRender            render;
	gl::Texture2D::shared_ptr		 glTexture1;
    gl::Texture2D::shared_ptr		 glTexture2;
	
	Impl()
	: optionsDesc("Allowed options"),
    running(false),
	initialized(false),
	surfDisplay(NULL),
	mainwindow(NULL),
	maincontext(NULL),
	texture(NULL),
	glslProgram(NULL),
	vtxShader(NULL),
	fragShader(NULL)
	{}
	
    void display();

    void reshape(int w, int h);

    void keyboard(int key, int x, int y);

    bool quitRequested();

    void onQuit();

    void loop();

    void dispatchEvent(SDL_Event * event);

    void initialize();

    void mainLoop();

	void restart();
    
    void changeColor();
    
    void fireParticle();
    
    void setupScene();
    
    void setupMaterial();
    
    void move(const Vector3 & delta);
    
    void print();
};


TestApp::TestApp(int argc, char** argv) 
: d(new TestApp::Impl())
{        
    init(argc, argv);
}

TestApp::~TestApp() {
    
}

void TestApp::init(int argc, char** argv) 
{
    using namespace std;
    using namespace core::utils;
    vector<string> default_search_dirs = { get_app_path() };
    // Declare the supported options.
    d->optionsDesc.add_options()
        ("help", "produce help message")
        ("interactive", po::value<bool>()->default_value(true), "True to run interactive test bed. False to run automated tests")
        ("resourcesdir", po::value<vector<string>>()->default_value(default_search_dirs, get_app_path()), "path used to load all resources")
        ;

    po::store(po::parse_command_line(argc, argv, d->optionsDesc), d->options);
    po::notify(d->options);
}

int TestApp::run() 
{
    if (d->options.count("help")) {
        std::cout << d->optionsDesc << "\n";
        return 1;
    }
	
    // Set Resources search directories
    for (auto path : (d->options["resourcesdir"].as<std::vector<std::string>>()))
    {
        ResourcesManager::addResourcesDirectory(path);
    }

    if (d->options.count("interactive") && d->options["interactive"].as<bool>()) 
    {
        // initialize gui
        d->initialize();
        d->mainLoop();
    }
    else
    {
        TRACE(info) << "Running tests";
        SubdivisionTest test;
        test.run();
    }
    
    return 0;
}

void TestApp::Impl::dispatchEvent(SDL_Event * event)
{
    switch(event->type) {
        case SDL_QUIT: {
            if (quitRequested()) {
                running = false;
            }
            break;
        }
        case SDL_WINDOWEVENT: {
            if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
                int w = event->window.data1;
                int h =event->window.data2;
                reshape( w ,h);
            }
            break;
        }
//        case SDL_VIDEOEXPOSE: {
//            display();
//            break;
//        }
        case SDL_KEYDOWN: {
            keyboard(event->key.keysym.sym, 0, 0);
        }
    }
}

bool TestApp::Impl::quitRequested()
{
    return true;
}

void TestApp::Impl::onQuit()
{
    /* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_FreeSurface(texture);
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();
}

void TestApp::Impl::loop()
{
	SDL_Delay(15);
}

void TestApp::Impl::keyboard(int key, int x, int y)
{
    TRACE(debug)  << "Key: " << (int)key ;
    switch (key) {
        case SDLK_ESCAPE:
            SDL_Event event;
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
            break;
		
        case SDLK_w:
		case SDLK_UP:
            move(Vector3(    0,    0, 0.5f));
			break;
        case SDLK_s:
        case SDLK_DOWN:
            move(Vector3(    0,    0,-0.5f));
            break;
        case SDLK_a:
        case SDLK_LEFT:
            move(Vector3(-0.5f,    0,    0));
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            move(Vector3( 0.5f,    0,    0));
            break;
        case SDLK_q:
            move(Vector3(    0,-0.5f,    0));
            break;
        case SDLK_e:
            move(Vector3(    0, 0.5f,    0));
            break;
		case SDLK_SPACE: {
			restart();
			reshape(640,480);
			break;
        case SDLK_p:
            print();
            break;
		}
			
        default:
            break;
    }
}

void TestApp::Impl::initialize()
{
    initialized = false;
    
	ResourcesManager rscMgr;

    /* Initialize SDL's Video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;// false;
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
        TRACE(debug) << "Unable to create window" ;
        return;
    }
    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
	if (!maincontext) {
		TRACE(debug) << "ERROR: Unable to create OpenGL context" <<  std::endl;
		return;
	}

    // Now that we have the window created with an apropriate GL context,
    // Setup the engine with O
    SDL_GL_MakeCurrent(mainwindow, maincontext);
    CLManager::instance()->setUseGPU(true);
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL
        | PlastilinaSubsystem::OPENCL
    	| PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
	if (!PlastilinaEngine::initialize(flags)) {
		return;
	}

	render.initialize();
    
	// Set up the rendering context, define display lists etc.:
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::string texturePath = rscMgr.findResourcePath("Texture01", "png");
	texture = IMG_Load(texturePath.c_str());
	if (!texture) {
		TRACE(debug) << "Failed to load texture" ;
		TRACE(debug) << "SDL Error: " << SDL_GetError() ;
        throw std::runtime_error("Failed to load asset: Texture01.png");
	} else {
		TRACE(debug) << "Texture loaded" ;
        TRACE(debug) << "TextureFormat: " << SDL_GetPixelFormatName(texture->format->format) ;
        TRACE(debug) << "Size " << texture->w << "x" << texture->h ;
        TRACE(debug) << "Bytes Per Pixel: " << int(texture->format->BytesPerPixel) ;

		glTexture1 = gl::Texture2D::shared_ptr(new gl::Texture2D());
        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        glTexture1->bind();
        glTexture1->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        glTexture1->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
		glTexture1->texImage2D(0,
            GL_RGBA8,
            texture->w,
            texture->h,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            texture->pixels);
        
        glTexture2 = gl::Texture2D::shared_ptr(new gl::Texture2D());
        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        glTexture2->bind();
        glTexture2->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        glTexture2->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
		glTexture2->texImage2D(0,
                                  GL_RGBA8,
                                  texture->w,
                                  texture->h,
                                  0,
                                  GL_RGB,
                                  GL_UNSIGNED_BYTE,
                                  texture->pixels);
	}
	
	
	restart();
	
	reshape(1280,720);
    
    initialized = true;
}

void TestApp::Impl::reshape(int w, int h)
{
#define DEFAULT_HEIGHT (4.0f)

    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
	auto camera = scene->getCamera();
    if (camera && camera->camera()) {
    	camera->camera()->setViewport(0, 0, w, h);
    }
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	SDL_GL_SwapWindow(mainwindow);
}

void TestApp::Impl::display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	scene->render();
	SDL_GL_SwapWindow(mainwindow);
    changeColor();
}

void TestApp::Impl::mainLoop()
{
    SDL_Event Event;
    running = true;
    while (running) {
        while (SDL_PollEvent(&Event)) {
            dispatchEvent(&Event);
        }

        loop();
        display();
    }

    onQuit();
}

void TestApp::Impl::restart() {
    Eigen::IOFormat octaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
    
    setupScene();
    setupMaterial();
    
    TRACE(debug)  << "Dump scene: \n";
    scene->dump();
    auto camera = scene->getCamera();
    if (camera && camera->camera()) {
        TRACE(debug) << "Camera modelview:"
          << camera->camera()->modelView().format(octaveFmt)
          << "Camera projection: \n"
          << camera->camera()->projection().format(octaveFmt);
    }
    object = camera;
}

void TestApp::Impl::setupScene() {
    static int counter = 0;
    counter++;
    
    std::string name =  std::string("Scene ") + std::to_string(counter);
    scene = std::make_shared<Scene>(name);
    
    bool useFile = false;
    if (useFile) {
        scene->loadFromFile("/Users/rcabral/Projects/qsculpt/assets/meshes/test2.dae");
    } else {
        ISurface * surf = core::PrimitiveFactory<core::GpuSubdivision>::createBox();
        //ISurface * surf = core::PrimitiveFactory<Subdivision>::createBox();
        SceneNode::shared_ptr node = std::make_shared<SurfaceNode>(surf, "Box");
        scene->add(node);
        Camera::shared_ptr cam = std::make_shared<Camera>();
        node = std::make_shared<CameraNode>();
        std::dynamic_pointer_cast<CameraNode>(node)->setCamera(cam);
        scene->add(node);
        node->transform() *= Eigen::Translation3f(0,0,3.0);
        cam->setPerspectiveMatrix(45.0f, 1280.0f/720.0f, 0.01f, 100.0f);
        cam->setTargetPoint(Point3(0,0,1));
        cam->setPosition(Point3(0,0,0));
        cam->setOrientationVector(Point3(0,1,0));
        cam->setViewport(0, 0, 1280, 720);
    }
}


void TestApp::Impl::setupMaterial()
{
    material = std::make_shared<PointMaterial>();
    material2 = std::make_shared<TestMaterial>();
    material3 = std::make_shared<PhongMaterial>();
    try {
        material->load();
		
        
        material2->load();
        material2->setDiffuse (Color(1.0f, 0.4f, 0.8f, 1.0f));
        material2->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material2->setAmbient (Color(0.1f, 0.1f, 0.1f, 1.0f));
        material2->setExponent(200);
        material2->setDiffuseTexture(glTexture1);
        render.setGLTexSrc(glTexture1);
        render.setGLTexDest(glTexture2);
		
        material3->load();
        material3->setDiffuse (Color(1.0f, 1.0f, 1.0f, 1.0f));
        material3->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material3->setAmbient (Color(0.1f, 0.1f, 0.1f, 1.0f));
        material3->setExponent(200);
        
        auto it = scene->treeIterator();
        while (it.hasNext()) {
            auto node = it.next();
            TRACE(debug)  << "Node: " << node->name() << " Type: " << node->nodeType();
            if (node->nodeType() == NT_Surface) {
                SurfaceNode::shared_ptr surface = std::static_pointer_cast<SurfaceNode>(node);
                surface->setMaterial(material2);
            }
        }
	} catch(core::GlException & e) {
        TRACE(debug)   << "GLException: " << e.what() 
        << e.error() << ": " << e.errorString() ;
    } catch (std::exception & e) {
        TRACE(debug) << "Exception: " << e.what() ;
    }
    
}


void TestApp::Impl::changeColor()
{
    render.render(0.5f);
    render.swapBuffers();
    auto ptr = render.glTexSrc();
    material2->setDiffuseTexture(ptr);
}

void TestApp::Impl::move(const Vector3 & delta)
{
    static Vector4 p = Vector4(0,0,0,1);
    
    p.x() += delta.x();
    p.y() += delta.y();
    p.z() += delta.z();
    
    if (object) {
        object->transform() *= Eigen::Translation3f(delta);
    }
}

void TestApp::Impl::print()
{
    static Eigen::IOFormat octaveFmt =
      Eigen::IOFormat(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
    
    auto camera = scene->getCamera();
    if (camera && camera->camera()) {
        //camera->transform() *= Eigen::Translation3f(delta);
        auto v = Vector4(0,0,0,1);
        Vector4 v2 = camera->camera()->modelView() * v;
        if ((v2[3] < -0.00001f) || (v2[3] > .00001f)) {
            v2 / v2.w();
        }
        TRACE(debug)  << "Point: \n" << v.format(octaveFmt)
          << "ModelView Matrix: \n" << camera->camera()->modelView().format(octaveFmt)
          << "v*M:\n" << v2.format(octaveFmt)
          << "Projection Matrix: \n" << camera->camera()->projection().format(octaveFmt);
        v = camera->camera()->projection() * v2;
        if  ((v[3] < -0.00001f) || (v[3] > .00001f)) {
            v / v.w();
        }
        v2 = camera->camera()->viewport() * v;
        TRACE(debug)  << "v*P: \n" << v.format(octaveFmt)
          << "Viewport Matrix: \n" << camera->camera()->viewport().format(octaveFmt)
          << "Viewport P: \n" << v2.format(octaveFmt);
        
    }
}

