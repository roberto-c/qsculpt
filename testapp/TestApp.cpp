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
#include <PlastilinaCore/ResourcesManager.h>
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
#include "BaseTest.h"
#include "CameraTest.h"
#include "SubdivisionTest.h"

using namespace std;
namespace po = boost::program_options;

struct TestApp::Impl {
    po::options_description optionsDesc;
    po::variables_map   options;
    bool                running;
    bool                initialized;
    SDL_Surface*        surfDisplay;
    SDL_Window*         mainwindow; /* Our window handle */
    SDL_GLContext       maincontext; /* Our opengl context handle */
    
    string              appName;

    vector<unique_ptr<BaseTest>> testList;

	Impl()
	: optionsDesc("Allowed options"),
    running(false),
	initialized(false),
	surfDisplay(NULL),
	mainwindow(NULL),
	maincontext(NULL)
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
        ("verbosity", po::value<boost::log::trivial::severity_level>()->default_value(boost::log::trivial::info), "verbosity level to print")
        ;

    po::store(po::parse_command_line(argc, argv, d->optionsDesc), d->options);
    po::notify(d->options);

    d->testList.push_back(unique_ptr<BaseTest>(new SubdivisionTest()));
    d->testList.push_back(unique_ptr<BaseTest>(new CameraTest()));
}

int TestApp::run() 
{
    if (d->options.count("help")) {
        std::cout << d->optionsDesc << "\n";
        return 1;
    }

    if (d->options.count("verbosity")) {
        auto verbosity = d->options["verbosity"].as<boost::log::trivial::severity_level>();
        boost::log::core::get()->set_filter
            (
                boost::log::trivial::severity >= verbosity
                );
    }
	
    // Set Resources search directories
    for (auto path : (d->options["resourcesdir"].as<std::vector<std::string>>()))
    {
        ResourcesManager::addResourcesDirectory(path);
    }

    //if (d->options.count("interactive") && d->options["interactive"].as<bool>()) 
    //{
    //    // initialize gui
    //    d->initialize();
    //    d->mainLoop();
    //}
    d->initialize();
    for (auto & test : d->testList)
    {
        TRACE(info) << "Test: " << test->name();
        test->setup();
        test->run();
        test->shutdown();
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
            break;
        }
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
            //event->button
            break;
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
			break;
        case SDLK_s:
        case SDLK_DOWN:
            break;
        case SDLK_a:
        case SDLK_LEFT:
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            break;
        case SDLK_q:
            break;
        case SDLK_e:
            break;
		case SDLK_SPACE: {
			break;
        case SDLK_p:
            break;
		}
			
        default:
            break;
    }
}

void TestApp::Impl::initialize()
{
    initialized = false;

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
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL
        | PlastilinaSubsystem::OPENCL
    	| PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
	if (!PlastilinaEngine::initialize(flags)) {
		return;
	}
		
	reshape(1280,720);
    
    initialized = true;
}

void TestApp::Impl::reshape(int w, int h)
{
#define DEFAULT_HEIGHT (4.0f)

    // setup viewport, projection etc. for OpenGL:
    glViewport( 0, 0, ( GLint ) w, ( GLint ) h );

	SDL_GL_SwapWindow(mainwindow);
}

void TestApp::Impl::display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	SDL_GL_SwapWindow(mainwindow);
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
