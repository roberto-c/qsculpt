//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "Stable.h"

#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>

#ifdef __APPLE__
#include <mach-o/dyld.h> // for application directory
#endif

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "BaseTest.h"
#include "CameraTest.h"
#include "CanvasTest.h"
#include "SubdivisionTest.h"
#include "TestApp.h"

using namespace std;
namespace po = boost::program_options;

struct TestApp::Impl
{
    TestApp*                app;
    po::options_description optionsDesc;
    po::variables_map       options;
    bool                    running;
    bool                    initialized;
    SDL_Surface*            surfDisplay;
    SDL_Window*             mainwindow;  /* Our window handle */
    SDL_GLContext           maincontext; /* Our opengl context handle */
    uint32_t                testEventType;

    string appName;

    vector<unique_ptr<BaseTest>>           testList;
    vector<unique_ptr<BaseTest>>::iterator currentTest;

    Impl()
        : app(nullptr)
        , optionsDesc("Allowed options")
        , running(false)
        , initialized(false)
        , surfDisplay(nullptr)
        , mainwindow(nullptr)
        , maincontext(nullptr)
        , testEventType(-1)
        , currentTest(testList.end())
    {
    }

    void display();

    void reshape(int w, int h);

    void keyboard(int key, int x, int y);

    void mouseClick(uint8_t button, uint8_t state, int x, int y);

    void mouseMotion(uint8_t state, int x, int y);

    bool quitRequested();

    void onQuit();

    void loop();

    void dispatchEvent(SDL_Event* event);

    void initialize();

    void mainLoop();

    void testEventHandler(const BaseTest* test, TestEvent evt,
                          void* userData);
};

TestApp::TestApp(int argc, char** argv)
    : d(new TestApp::Impl())
{
    d->app = this;
    init(argc, argv);
}

TestApp::~TestApp() {}

void TestApp::init(int argc, char** argv)
{
    using namespace std;
    using namespace core::utils;
    vector<string> default_search_dirs = {get_app_path()};
    // Declare the supported options.
    d->optionsDesc.add_options()("help", "produce help message")(
        "interactive", po::value<bool>()->default_value(true),
        "True to run interactive test bed. False to run automated tests")(
        "resourcesdir",
        po::value<vector<string>>()->default_value(default_search_dirs,
                                                   get_app_path()),
        "path used to load all resources")(
        "verbosity",
        po::value<boost::log::trivial::severity_level>()->default_value(
            boost::log::trivial::info),
        "verbosity level to print")(
        "testid", po::value<int>()->default_value(-1), "test to execute");

    po::store(po::parse_command_line(argc, argv, d->optionsDesc), d->options);
    po::notify(d->options);

    auto testid = d->options["testid"].as<int>();

    if (testid == -1 || testid == 0)
        d->testList.push_back(unique_ptr<BaseTest>(new SubdivisionTest()));
    if (testid == -1 || testid == 1)
        d->testList.push_back(unique_ptr<BaseTest>(new CameraTest()));
    if (testid == -1 || testid == 2)
        d->testList.push_back(unique_ptr<BaseTest>(new CanvasTest()));

    // install test callsback
    for (auto& test : d->testList)
    {
        test->setNotifyCallback(
            [](const BaseTest* test, TestEvent evt, void* userData) -> void {
                assert(test && "Test cannot be null");
                assert(userData && "Expected to have userData set");
                // TRACE(trace) << "Test Name: " << test->name() << "Event: "
                // << to_string(evt);
                TestApp* app = static_cast<TestApp*>(userData);

                SDL_Event myEvent;
                SDL_memset(&myEvent, 0, sizeof(SDL_Event));
                myEvent.type       = app->d->testEventType;
                myEvent.user.code  = static_cast<uint32_t>(evt);
                myEvent.user.data1 = (void*)(test);
                myEvent.user.data2 = userData;
                SDL_PushEvent(&myEvent);
            },
            (void*)this);
    }
    d->currentTest = d->testList.end();
}

int TestApp::run()
{
    if (d->options.count("help"))
    {
        std::cout << d->optionsDesc << "\n";
        return 1;
    }

    if (d->options.count("verbosity"))
    {
        auto verbosity =
            d->options["verbosity"].as<boost::log::trivial::severity_level>();
        boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                            verbosity);
    }

    d->initialize();
    d->mainLoop();

    return 0;
}

void TestApp::Impl::dispatchEvent(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_QUIT:
    {
        if (quitRequested())
        {
            running = false;
        }
        break;
    }
    case SDL_WINDOWEVENT:
    {
        if (event->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            int w = event->window.data1;
            int h = event->window.data2;
            reshape(w, h);
        }
        break;
    }
    //        case SDL_VIDEOEXPOSE: {
    //            display();
    //            break;
    //        }
    case SDL_KEYDOWN:
    {
        keyboard(event->key.keysym.sym, 0, 0);
        break;
    }
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    {
        mouseClick(event->button.button, event->button.state, event->button.x,
                   event->button.y);
        break;
    }
    case SDL_MOUSEMOTION:
        mouseMotion(event->motion.state, event->motion.x, event->motion.y);
        break;
    }
    if (event->type != -1 && event->type == testEventType)
    {
        testEventHandler(static_cast<BaseTest*>(event->user.data1),
                         static_cast<TestEvent>(event->user.code),
                         event->user.data2);
    }
}

bool TestApp::Impl::quitRequested() { return true; }

void TestApp::Impl::onQuit()
{
    std::cout << std::endl;
    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();
}

void TestApp::Impl::loop() { SDL_Delay(15); }

void TestApp::Impl::keyboard(int key, int x, int y)
{
    TRACE(debug) << "Key: " << (int)key;
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->keyboard(key, x, y);
        }
    }
    switch (key)
    {
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
    case SDLK_SPACE:
        // post event to start testing
        SDL_Event myEvent;
        SDL_memset(&myEvent, 0, sizeof(SDL_Event));
        myEvent.type = testEventType;
        myEvent.user.code =
            static_cast<uint32_t>(TestEvent::TE_SHUTDOWN_POST);
        myEvent.user.data1 = (void*)(nullptr);
        myEvent.user.data2 = (void*)(this->app);
        SDL_PushEvent(&myEvent);
        break;
    case SDLK_p:
        break;

    default:
        break;
    }
}

void TestApp::Impl::mouseClick(uint8_t button, uint8_t state, int x, int y)
{
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->mouseClick(button, state, x, y);
        }
    }
}

void TestApp::Impl::mouseMotion(uint8_t state, int x, int y)
{
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->mouseMove(state, x, y);
        }
    }
}

void TestApp::Impl::initialize()
{
    initialized = false;

    /* Initialize SDL's Video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        TRACE(error) << "Failed to intialize SDL";
        return; // false;
    }

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Request opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of
     * writing, but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    /* Create our window centered at 512x512 resolution */
    mainwindow = SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 1280, 720,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mainwindow)
    {
        /* Die if creation failed */
        TRACE(error) << "Unable to create window";
        return;
    }
    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
    if (!maincontext)
    {
        TRACE(error) << "Unable to create OpenGL context";
        return;
    }

    // Now that we have the window created with an apropriate GL context,
    // Setup the engine with O
    SDL_GL_MakeCurrent(mainwindow, maincontext);
    PlastilinaSubsystem flags = PlastilinaSubsystem::OPENGL |
                                PlastilinaSubsystem::OPENCL |
                                PlastilinaSubsystem::ENABLE_CL_GL_SHARING;
    if (!PlastilinaEngine::initialize(flags))
    {
        TRACE(error) << "Unable to initialize PlastilinaEngine";
        return;
    }

    std::string configFile = core::utils::get_app_path() + "\\config.cfg";
    if (!PlastilinaEngine::initializeFromConfigFile(configFile))
    {
        TRACE(error)
            << "Failed to initilize with config file. Using defaults.";
    }

    // Set Resources search directories
    for (auto path : (options["resourcesdir"].as<std::vector<std::string>>()))
    {
        ResourcesManager::addResourcesDirectory(path);
    }

    reshape(1280, 720);

    testEventType = SDL_RegisterEvents(1);

    // post event to start testing
    SDL_Event myEvent;
    SDL_memset(&myEvent, 0, sizeof(SDL_Event));
    myEvent.type       = testEventType;
    myEvent.user.code  = static_cast<uint32_t>(TestEvent::TE_SHUTDOWN_POST);
    myEvent.user.data1 = (void*)(nullptr);
    myEvent.user.data2 = (void*)(this->app);
    SDL_PushEvent(&myEvent);

    initialized = true;
}

void TestApp::Impl::reshape(int w, int h)
{
    SDL_GL_MakeCurrent(mainwindow, maincontext);
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->resize(w, h);
        }
    }
    SDL_GL_SwapWindow(mainwindow);
}

void TestApp::Impl::display()
{
    static double   updatelapse = 0;
    static uint32_t frameCtr    = 0;
    frameCtr++;
    uint64_t t1 = SDL_GetPerformanceCounter();

    SDL_GL_MakeCurrent(mainwindow, maincontext);
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->display();
        }
    }
    SDL_GL_SwapWindow(mainwindow);
    uint64_t t2     = SDL_GetPerformanceCounter();
    double   tdelta = double(t2 - t1) / double(SDL_GetPerformanceFrequency());
    updatelapse += tdelta;
    if (updatelapse > 0.1)
    {
        double fps  = frameCtr / updatelapse;
        updatelapse = 0;
        frameCtr    = 0;
        std::cout << std::unitbuf << std::fixed << std::setw(10)
                  << std::setprecision(1) << "\b\b\b\b\b\b\b\b\b\b" << fps;
    }
}

void TestApp::Impl::mainLoop()
{
    SDL_Event Event;
    running = true;
    while (running)
    {
        while (SDL_PollEvent(&Event))
        {
            dispatchEvent(&Event);
        }

        loop();
        display();
    }

    onQuit();
}

void TestApp::Impl::testEventHandler(const BaseTest* test, TestEvent evt,
                                     void* userData)
{
    assert(userData && "Expected to have userData set");
    // TRACE(trace) << ">> Test Name: " << std::string(test ? test->name() :
    // "nullptr") << "Event: " << to_string(evt);
    TestApp* app      = static_cast<TestApp*>(userData);
    bool     isUiTest = false;
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        isUiTest    = uitest != nullptr;
    }
    // TRACE(trace) << "Test Type: " << std::string(isUiTest ? "UiTest" :
    // "BaseTest");
    if (test == nullptr && evt == TestEvent::TE_SHUTDOWN_POST)
    {
        app->d->currentTest = app->d->testList.begin();
        if (app->d->currentTest != app->d->testList.end())
        {
            SDL_GL_MakeCurrent(mainwindow, maincontext);
            (*(app->d->currentTest))->setup();
            auto test   = (*(app->d->currentTest)).get();
            auto uitest = dynamic_cast<BaseUITest*>(test);
            isUiTest    = uitest != nullptr;
        }
        // TRACE(trace) << "<< Test Name: " << std::string(test ? test->name()
        // : "nullptr") << "Event: " << to_string(evt);
        return;
    }
    switch (evt)
    {
    case TestEvent::TE_SHUTDOWN_POST:
    {
        ++app->d->currentTest;
        if (app->d->currentTest == app->d->testList.end())
            break;
        (*(app->d->currentTest))->setup();
        break;
    }
    case TestEvent::TE_SETUP_POST:
    {
        (*(app->d->currentTest))->run();
        break;
    }
    case TestEvent::TE_RUN_POST:
    {
        if (!isUiTest)
        {
            (*(app->d->currentTest))->shutdown();
        }
        break;
    }
    case TestEvent::TE_RUN_FRAME_POST:
        break;
    case TestEvent::TE_RUN_TEST_END:
        (*(app->d->currentTest))->shutdown();
        break;
    }
    // TRACE(trace) << "<< Test Name: " << std::string(test ? test->name() :
    // "nullptr") << "Event: " << to_string(evt);
}
