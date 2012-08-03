//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <SDL/SDL.h>
//#include <GLUT/GLUT.h>
#include <OpenCL/opencl.h>
#include "cl.hpp"

#include <CoreEngine/Plastilina.h>
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

#include "CoreEngine/Vertex.h"
#include "CoreEngine/HEdge.h"
#include "CoreEngine/Face.h"
#include "CoreEngine/Point3D.h"
#include "CoreEngine/Octree.h"
#include "CoreEngine/Vector.h"
#include "CoreEngine/Scene.h"
#include "CoreEngine/SceneNode.h"
#include "CoreEngine/subdivision/Sphere.h"
#include "CoreEngine/subdivision/Box.h"
#include "CoreEngine/subdivision/Subdivision.h"
#include "CoreEngine/geometry/Sphere.h"
#include "CoreEngine/geometry/Ray.h"
#include "CoreEngine/FlatRenderer.h"
#include "CoreEngine/Camera.h"

#include "CoreEngine/GlslShader.h"
#include "CoreEngine/GlslProgram.h"
#include "CoreEngine/Color.h"


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
    std::string p("/Users/rcabral/sample2.bmp"); //p(exepath.data());
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
    SDL_Surface*        surfDisplay;
    std::string         appName;
    Scene::shared_ptr       scene;
    IRenderer           *renderer;
    Camera              camera;

    GlslProgram     *glslProgram;
    VertexShader    *vtxShader;
    FragmentShader  *fragShader;
};

TestApp::TestApp(int argc, char** argv) 
: d(new TestApp::Impl())
{        
    init(argc, argv);
}

TestApp::~TestApp() {
    
}

int TestApp::run() {
    
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
    if(event->type == SDL_QUIT) {
        if (quitRequested()) {
            d->running = false;
        }
    }
}

bool TestApp::quitRequested()
{
    return true;
}

void TestApp::onQuit()
{
    SDL_Quit();
}

void TestApp::loop()
{
    
}

void TestApp::keyboard(unsigned char key, int x, int y)
{
//    std::cout << "Key: " << (int)key << std::endl;
//    switch (key) {
//        case 27:
//            glutDestroyWindow(d->winId);
//            exit(0);
//            break;
//            
//        default:
//            break;
//    }
}

void TestApp::init(int argc, char** argv) {
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return;// false;
    }
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,            32);
    
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,        8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,        8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
    
    if((d->surfDisplay = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) {
        return;// false;
    }
    
    
    d->scene = std::make_shared<Scene>();
    
    SurfaceNode::shared_ptr n = std::make_shared<SurfaceNode>(new ::Box());
    n->surface()->setColor(Color(1.0, 0, 0, 1));
    d->scene->add(n);
    d->renderer = new FlatRenderer;
    d->camera.setTargetPoint( Point3( 0, 0, 0) );
    d->camera.setOrientationVector( Point3( 0, 0, 1) );
    d->camera.setPosition( Point3( 5.75, 3.75, -2.75) );
    
    try {
        d->glslProgram = new GlslProgram();
        d->vtxShader = new VertexShader();
        d->fragShader = new FragmentShader();
        
        d->glslProgram->attachShader(d->vtxShader);
        d->glslProgram->attachShader(d->fragShader);
        
        std::string filename = get_app_path();
        
        d->vtxShader->loadFromFile(filename + "/../Resources/Sample.vs");
        bool vtxcompile = d->vtxShader->compile();
        std::cout << "vtxcompile is: " << vtxcompile << std::endl;
        std::cout << "Vertex shader compilation log:" << std::endl;
        std::cout << d->vtxShader->infoLog() << std::endl;
        
        
        d->fragShader->loadFromFile(filename + "/../Resources/Sample.fs");
        bool fragcompile = d->fragShader->compile();
        std::cout << "fragcompile is: " << fragcompile << std::endl;
        std::cout   << "Fragment shader compilation log:" << std::endl
            << d->fragShader->infoLog() << std::endl;
        
        
        bool link = d->glslProgram->link();
        std::cout << "Link is: " << link << std::endl;
        std::cout   << "Link info log:" << std::endl 
            << d->glslProgram->buildLog() << std::endl;
        
        
    } catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void TestApp::reshape(int w, int h)
{
#define DEFAULT_HEIGHT (2.5f)
    
    // Set up the rendering context, define display lists etc.:
    //glClearColor( 0.4, 0.4, 0.4, 1.0 );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    
    float aspectRatio = float(w) / float(h);
    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );
    d->camera.setViewport(0, 0, w, h);
    d->camera.setPerspectiveMatrix(45.0, aspectRatio, 1.0, 50.0);
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixf(d->camera.projection().data());
    
    glMatrixMode( GL_MODELVIEW );
}

void TestApp::display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glEnable(GL_LIGHT0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(d->camera.modelView().data());
    
    try {
        //d->glslProgram->useProgram();
        Iterator<SceneNode> it = d->scene->constIterator();
        while (it.hasNext()) {
            auto n = it.next();
            auto s = n ? std::dynamic_pointer_cast<SurfaceNode>(n) : nullptr;
            if (s) {
                d->renderer->renderObject(s->surface());
            }
        }
    } catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    }
    
    SDL_GL_SwapBuffers();
}