//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <PlastilinaCore/Plastilina.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2_image/SDL_image.h>

#include <mach-o/dyld.h> // for application directory

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
//#include <omp.h>


#include "TestApp.h"

#include "PlastilinaCore/Vertex.h"
#include "PlastilinaCore/HEdge.h"
#include "PlastilinaCore/Face.h"
#include "PlastilinaCore/Point3D.h"
#include "PlastilinaCore/Octree.h"
#include "PlastilinaCore/Vector.h"
#include "PlastilinaCore/Scene.h"
#include "PlastilinaCore/SceneNode.h"
#include "PlastilinaCore/subdivision/Sphere.h"
#include "PlastilinaCore/subdivision/Box.h"
#include "PlastilinaCore/subdivision/Subdivision.h"
#include "PlastilinaCore/geometry/Sphere.h"
#include "PlastilinaCore/geometry/Ray.h"
#include "PlastilinaCore/FlatRenderer.h"
#include "PlastilinaCore/SmoothRenderer.h"
#include <PlastilinaCore/PointRenderer.h>
#include "PlastilinaCore/Camera.h"
#include <PlastilinaCore/BOManager.h>

#include <PlastilinaCore/opencl/OCLManager.h>
#include "PlastilinaCore/opengl/GlslShader.h"
#include "PlastilinaCore/opengl/GlslProgram.h"
#include <PlastilinaCore/physics/SimSystem.h>
#include <PlastilinaCore/physics/Actor.h>
#include <PlastilinaCore/physics/ForceFunctors.h>
#include "PlastilinaCore/Color.h"
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/material/PointMaterial.h>
#include <PlastilinaCore/pointcloud/PointCloud.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/opengl/Texture.h>

#include "Subdivision.h"
#include "ParticleSystem.h"
#include "CLRender.h"
#include "TestMaterial.h"

std::string get_app_path() {
    std::vector<char> exepath;
    uint32_t size = 0;
    _NSGetExecutablePath(NULL, &size);
    if (size > 0) {
        exepath.resize(size + 1);
        if (_NSGetExecutablePath(exepath.data(), &size) != 0)
            exepath[0]='\0';
    } else {
        exepath.push_back('\0');
    }
    std::string p(exepath.data());
    boost::filesystem::path path(p);
    
    if (boost::filesystem::is_regular_file(path)) {
        return path.parent_path().native();
    } else if (boost::filesystem::is_directory(path)) {
        return path.native();
    }
    
    return std::string();
}

struct TestApp::Impl {
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
	: running(false),
	initialized(false),
	surfDisplay(NULL),
	mainwindow(NULL),
	maincontext(NULL),
	texture(NULL),
	glslProgram(NULL),
	vtxShader(NULL),
	fragShader(NULL)
	{}
	
	void restart();
    
    void changeColor();
    
    void fireParticle();
    
    void setupScene();
    
    void setupMaterial();
    
    void move(const Vector3 & delta);
};


TestApp::TestApp(int argc, char** argv) 
: d(new TestApp::Impl())
{        
    init(argc, argv);
}

TestApp::~TestApp() {
    
}

int testfunc(int x, int y) {
    return 2 + x + y;
}

struct testfunctor : public std::binary_function<int, int, int> {
    std::string name;
    testfunctor(const std::string & name = "") : name(name) {};
    int operator()(int x, int y) {
        std::cout << "name: " << name << "\n";
        return 4 + x + y;
    }
};

int TestApp::run() {
    if (!d->initialized) {
        std::cerr << "Application failed to initialized." << std::endl;
        return 1;
    }
	
    SDL_Event Event;
    d->running = true;
    while(d->running) {
        while(SDL_PollEvent(&Event)) {
            dispatchEvent(&Event);
        }
        
        loop();
        display();
    }
    
    onQuit();
    
    return 0;
    //glutMainLoop();
}

void TestApp::dispatchEvent(SDL_Event * event)
{
    switch(event->type) {
        case SDL_QUIT: {
            if (quitRequested()) {
                d->running = false;
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

bool TestApp::quitRequested()
{
    return true;
}

void TestApp::onQuit()
{
    /* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_FreeSurface(d->texture);
    SDL_GL_DeleteContext(d->maincontext);
    SDL_DestroyWindow(d->mainwindow);
    SDL_Quit();
}

void TestApp::loop()
{
	SDL_Delay(15);
}

void TestApp::keyboard(int key, int x, int y)
{
    std::cout << "Key: " << (int)key << std::endl;
    switch (key) {
        case SDLK_ESCAPE:
            SDL_Event event;
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
            break;
		
        case SDLK_w:
		case SDLK_UP:
            d->move(Vector3(    0,    0, 0.5f));
			break;
        case SDLK_s:
        case SDLK_DOWN:
            d->move(Vector3(    0,    0,-0.5f));
            break;
        case SDLK_a:
        case SDLK_LEFT:
            d->move(Vector3(-0.5f,    0,    0));
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            d->move(Vector3( 0.5f,    0,    0));
            break;
        case SDLK_q:
            d->move(Vector3(    0,-0.5f,    0));
            break;
        case SDLK_e:
            d->move(Vector3(    0, 0.5f,    0));
            break;
		case SDLK_SPACE: {
			d->restart();
			reshape(640,480);
			break;
		}
			
        default:
            break;
    }
}

void TestApp::init(int argc, char** argv) {
    d->initialized = false;

	std::string app_path = get_app_path();
	std::cout << "App path: " << app_path << std::endl;
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
    d->mainwindow = SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!d->mainwindow) {
        /* Die if creation failed */
        std::cerr << "Unable to create window" << std::endl;
        return;
    }
    /* Create our opengl context and attach it to our window */
    d->maincontext = SDL_GL_CreateContext(d->mainwindow);
	if (!d->maincontext) {
		std::cerr << "ERROR: Unable to create OpenGL context" <<  std::endl;
		return;
	}
    // Now that we have the window created with an apropriate GL context,
    // Setup the engine with O
    //CLManager::instance()->setUseGPU(false);
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL |
    							PlastilinaSubsystem::OPENCL |
    							PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
	if (!PlastilinaEngine::initialize(flags)) {
		return;
	}
	
	d->render.initialize();
    
	// Set up the rendering context, define display lists etc.:
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::string texture = rscMgr.findResourcePath("Texture01", "png");
	d->texture = IMG_Load(texture.c_str());
	if (!d->texture) {
		std::cerr << "Failed to load texture" << std::endl;
		std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
	} else {
		std::cerr << "Texture loaded" << std::endl;
		std::cerr << "Size " << d->texture->w << "x" << d->texture->h << std::endl;
		d->glTexture1 = gl::Texture2D::shared_ptr(new gl::Texture2D());
        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        d->glTexture1->bind();
        d->glTexture1->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        d->glTexture1->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
		d->glTexture1->texImage2D(GL_TEXTURE_2D,
                                 0,
                                 GL_RGBA8,
                                 d->texture->w,
                                 d->texture->h,
                                 0,
                                 GL_BGRA,
                                 GL_UNSIGNED_BYTE,
                                 d->texture->pixels);
        
        d->glTexture2 = gl::Texture2D::shared_ptr(new gl::Texture2D());
        gl::TextureManager::instance()->setActiveTexture(GL_TEXTURE0);
        d->glTexture2->bind();
        d->glTexture2->setParameter(GL_TEXTURE_BASE_LEVEL, 0);
        d->glTexture2->setParameter(GL_TEXTURE_MAX_LEVEL, 0);
		d->glTexture2->texImage2D(GL_TEXTURE_2D,
                                  0,
                                  GL_RGBA8,
                                  d->texture->w,
                                  d->texture->h,
                                  0,
                                  GL_BGRA,
                                  GL_UNSIGNED_BYTE,
                                  d->texture->pixels);
	}
	
	
	d->restart();
	
	reshape(1280,720);
    
    d->initialized = true;
}

void TestApp::reshape(int w, int h)
{
#define DEFAULT_HEIGHT (4.0f)

    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
	auto camera = d->scene->getCamera()->camera();
    camera->setViewport(0, 0, w, h);
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	SDL_GL_SwapWindow(d->mainwindow);
}

void TestApp::display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	d->scene->render();
	SDL_GL_SwapWindow(d->mainwindow);
    d->changeColor();
}

void TestApp::Impl::restart() {
    static int counter = 0;
	counter++;
    Eigen::IOFormat octaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
    
    std::string name =  std::string("Scene ") + std::to_string(counter);
	scene = std::make_shared<Scene>(name);
    
    scene->loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/assets/meshes/test2.dae");
    setupMaterial();
    
    std::cout << "Dump scene: \n";
    scene->dump();
    auto camera = scene->getCamera();
    if (camera && camera->camera()) {
        std::cout << "Camera modelview: \n";
        std::cout << camera->camera()->modelView().format(octaveFmt) << "\n";
        std::cout << "Camera projection: \n";
        std::cout << camera->camera()->projection().format(octaveFmt);
        std::cout << "\n";
    }
    object = camera;
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
        material3->setDiffuse (Color(1.0f, 0.4f, 0.8f, 1.0f));
        material3->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material3->setAmbient (Color(0.1f, 0.1f, 0.1f, 1.0f));
        material3->setExponent(200);
        
        auto it = scene->treeIterator();
        while (it.hasNext()) {
            auto node = it.next();
            std::cout << "Node: " << node->name() << " Type: " << node->nodeType() << "\n";
            if (node->nodeType() == NT_Surface) {
                SurfaceNode::shared_ptr surface = std::static_pointer_cast<SurfaceNode>(node);
                surface->setMaterial(material3);
            }
        }
	} catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
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
    static Eigen::IOFormat octaveFmt =
    		Eigen::IOFormat(Eigen::StreamPrecision, 0, ", ", ";", "", "", "[", "]");
    
    p.x() += delta.x();
    p.y() += delta.y();
    p.z() += delta.z();
    
 	object->transform() *= Eigen::Translation3f(delta);
    
    auto camera = scene->getCamera();
    if (camera && camera->camera()) {
        auto v = p;
        std::cout << "Point: " << v.format(octaveFmt) << "\n";
        std::cout << "ModelView Matrix: " << camera->camera()->modelView().format(octaveFmt) << "\n";
        v = camera->camera()->modelView() * v;
        if  ((v[3] < -0.00001f) || (v[3] > .00001f)) {
            v / v.w();
        }
        std::cout << "v*M: " << v.format(octaveFmt) << "\n";
        std::cout << "Projection Matrix: " << camera->camera()->projection().format(octaveFmt) << "\n";
        v = camera->camera()->projection() * v;
        if  ((v[3] < -0.00001f) || (v[3] > .00001f)) {
            v / v.w();
        }
        std::cout << "v*P: " << v.format(octaveFmt) << "\n";
        v = camera->camera()->viewport() * v;
        std::cout << "Viewport P: " << v.format(octaveFmt) << "\n";
        
    }
}

