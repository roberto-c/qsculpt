//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <SDL2/SDL.h>

#include <PlastilinaCore/Plastilina.h>
#include <mach-o/dyld.h> // for application directory

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <boost/filesystem.hpp>
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
#include "PlastilinaCore/Color.h"
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/material/PointMaterial.h>

#include "Subdivision.h"
#include "ParticleSystem.h"

#include "ParticleSystem.cl.h"

#include "CLRender.h"

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
    
    std::string         appName;
    Scene::shared_ptr           scene;

    GlslProgram     *glslProgram;
    VertexShader    *vtxShader;
    FragmentShader  *fragShader;
    
    std::shared_ptr<PointMaterial>       material;
    std::shared_ptr<PhongMaterial>       material2;
	
	CLRender            render;
	
	void restart();
};


TestApp::TestApp(int argc, char** argv) 
: d(new TestApp::Impl())
{        
    init(argc, argv);
}

TestApp::~TestApp() {
    
}

int TestApp::run() {
    if (!d->initialized) {
        std::cerr << "Applicatoin failed to initialized." << std::endl;
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
    SDL_GL_DeleteContext(d->maincontext);
    SDL_DestroyWindow(d->mainwindow);
    SDL_Quit();
}

void TestApp::loop()
{
	//d->scene->transform().translate(Vector3(0.01f,0.01f,0.01f));
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
			
		case SDLK_UP: {
				auto n = d->scene->item(0).lock();
				SurfaceNode::shared_ptr s = std::static_pointer_cast<SurfaceNode>(n);
				if (s) {
					subdivide(s);
//					ParticleSystem_step(s, 0.1);
				}
			}
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

    /* Initialize SDL's Video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;// false;
    }
    
	CLManager::instance()->setUseGPU(false);
	if (!PlastilinaEngine::initialize(PS_OPENGL|PS_OPENCL)) {
		return;
	}
	
	d->render.initialize();
    
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
                                  640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!d->mainwindow) {
        /* Die if creation failed */
        std::cerr << "Unable to create window" << std::endl;
        return;
    }
    /* Create our opengl context and attach it to our window */
    d->maincontext = SDL_GL_CreateContext(d->mainwindow);

	// Set up the rendering context, define display lists etc.:
    //glClearColor( 0.4, 0.4, 0.4, 1.0 );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

	d->restart();
	
	reshape(640,480);
	
    d->initialized = true;
}

void TestApp::reshape(int w, int h)
{
#define DEFAULT_HEIGHT (2.5f)

    float aspectRatio = float(w) / float(h);
    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
	auto camera = d->scene->getCamera()->camera();
    camera->setViewport(0, 0, w, h);
    camera->setPerspectiveMatrix(45.0, aspectRatio, 1.0, 50.0);
    camera->setTargetPoint( Point3( 0, 0, 0) );
    camera->setOrientationVector( Point3( 0, 1, 0) );
    camera->setPosition( Point3( 0.0f, 2.0f, -10.0f) );
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	SDL_GL_SwapWindow(d->mainwindow);
}

void TestApp::display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//	glClear( GL_DEPTH_BUFFER_BIT );
    
//	d->scene->render();
	d->render.render(0);
	
	SDL_GL_SwapWindow(d->mainwindow);
}

void TestApp::Impl::restart() {
	static int counter = 0;
	counter++;
	std::string name =  std::string("Scene ") + std::to_string(counter);
	scene = std::make_shared<Scene>(name);
    
    SurfaceNode::shared_ptr n = std::make_shared<SurfaceNode>(new ::Plane());
    n->surface()->setColor(Color(0.0, 1.0, 0, 0.2f));
	SurfaceNode::shared_ptr n2 = std::make_shared<SurfaceNode>(new ::Plane());
    n2->surface()->setColor(Color(1.0, 0.6f, 0.5f, 0.2f));
    scene->add(n);
	//	d->scene->add(n2);
	n2->transform() *= Eigen::Translation3f(Eigen::Vector3f(3,0,0));
    CameraNode::shared_ptr cam = scene->createCamera();
    scene->add(cam);
    material = std::make_shared<PointMaterial>();
    material2 = std::make_shared<PhongMaterial>();
    try {
        material->load();
		
        
        material2->load();
        material2->setDiffuse (Color(1.0f, 0.4f, 0.8f, 1.0f));
        material2->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material2->setAmbient (Color(1.0f, 0.1f, 0.1f, 1.0f));
        material2->setExponent(200);
		
        n2->setMaterial(material);
		n->setMaterial(material2);
	} catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}
