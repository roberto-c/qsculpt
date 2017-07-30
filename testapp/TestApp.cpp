//
//  TestApp.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "Stable.h"

#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/ResourcesManager.h>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <PlastilinaCore/opengl/OpenGL.h>

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
#include "DeviceSelectionTest.h"
#include "SubdivisionTest.h"
#include "TestApp.h"

using namespace std;
using namespace core;
namespace po = boost::program_options;

struct TestApp::Impl
{
    TestApp*                app;
    int                     argc;
    char**                  argv;
    po::options_description optionsDesc;
    po::variables_map       options;
    bool                    initialized;
    bool                    running;
    SDL_Surface*            surfDisplay;
    SDL_Window*             mainwindow;  /* Our window handle */
    SDL_GLContext           maincontext; /* Our opengl context handle */
    uint32_t                testEventType;
    IGraphicsContext*       context;

    string appName;

    vector<unique_ptr<BaseTest>>           testList;
    vector<unique_ptr<BaseTest>>::iterator currentTest;

    Impl()
        : app(nullptr)
        , argc(0)
        , argv(nullptr)
        , optionsDesc("Allowed options")
        , initialized(false)
        , running(false)
        , surfDisplay(nullptr)
        , mainwindow(nullptr)
        , maincontext(nullptr)
        , testEventType(-1)
        , currentTest(testList.end())
        , context(nullptr)
    {
    }

    void parseOptions(int argc, char** argv);

    bool init(int argc, char** argv);

    void display();

    void reshape(int w, int h);

    void keyboard(int key, int x, int y);

    void mouseClick(uint8_t button, uint8_t state, int x, int y);

    void mouseMotion(uint8_t state, int x, int y);

    bool quitRequested();

    void onQuit();

    void loop();

    void dispatchEvent(SDL_Event* event);

    void mainLoop();

    void testEventHandler(const BaseTest* test, TestEvent evt,
                          void* userData);
};

TestApp::TestApp(int argc, char** argv)
    : d(std::make_unique<TestApp::Impl>())
{
    d->app = this;
    d->argc = argc;
    d->argv = argv;
}

TestApp::~TestApp() {}

int TestApp::run()
{
    d->parseOptions(d->argc, d->argv);

    if (d->options.count("help"))
    {
        std::cout << d->optionsDesc << "\n";
        PlastilinaEngine::printCommandLineOptionsHelp();
        return 1;
    }
    d->initialized = d->init(d->argc, d->argv);
    if (!d->initialized)
    {
        return 2;
    }
    d->mainLoop();

    return 0;
}

void TestApp::Impl::parseOptions(int argc, char** argv)
{
    // Declare the supported options.
    optionsDesc.add_options()("help", "produce help message")(
        "interactive", po::value<bool>()->default_value(true),
        "True to run interactive test bed. False to run automated tests")(
        "verbosity",
        po::value<boost::log::trivial::severity_level>()->default_value(
            boost::log::trivial::info),
        "verbosity level to print")(
        "testid", po::value<int>()->default_value(-1), "test to execute");

    po::parsed_options parsed = po::command_line_parser(argc, argv)
        .options(optionsDesc)
        .allow_unregistered()
        .run();
    po::store(parsed, options);
    po::notify(options);
}

bool TestApp::Impl::init(int argc, char** argv)
{
    using namespace std;
    using namespace core::utils;

    auto testid = options["testid"].as<int>();

    if (testid == -1 || testid == 0)
        testList.push_back(unique_ptr<BaseTest>(new DeviceSelectionTest()));
    if (testid == -1 || testid == 1)
        testList.push_back(unique_ptr<BaseTest>(new SubdivisionTest()));
    if (testid == -1 || testid == 2)
        testList.push_back(unique_ptr<BaseTest>(new CameraTest()));
    if (testid == -1 || testid == 3)
        testList.push_back(unique_ptr<BaseTest>(new CanvasTest()));

    // install test callsback
    for (auto& test : testList)
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
            (void*)this->app);
    }
    currentTest = testList.end();

    if (options.count("verbosity"))
    {
        auto verbosity =
            options["verbosity"].as<boost::log::trivial::severity_level>();
        boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                            verbosity);
    }

    if (!PlastilinaEngine::initializeFromCommandLine(argc, argv))
    {
        TRACE(error) << "Failed to parse command line";
    }
    std::string configFile = core::utils::get_app_path() + "\\config.cfg";
    if (!PlastilinaEngine::initializeFromConfigFile(configFile))
    {
        TRACE(error)
            << "Failed to initilize with config file. Using defaults.";
    }

    auto options = PlastilinaEngine::options();
    for (auto it : options)
    {
        TRACE(info) << "Name: " << it.first;
    }

    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        TRACE(error) << "Failed to intialize SDL";
        return false;
    }
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(imgFlags) != imgFlags)
    {
        TRACE(error) << "Failed to intialize SDL_image";
        return false;
    }

    // Create our window centered at 512x512 resolution
    mainwindow = SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 1280, 720,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mainwindow)
    {
        TRACE(error) << "Unable to create window";
        return false;
    }
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (!SDL_GetWindowWMInfo(mainwindow, &wmInfo))
    {
        TRACE(error) << "Failed to get window system information";
        return false;
    }
    GraphicsContextCreateInfo createInfo;
    createInfo.contextType = ContextType::OpenGL;
#if WIN32
    createInfo.osWindowHandle = reinterpret_cast<intptr_t>(wmInfo.info.win.window);
    createInfo.osHandleEx = reinterpret_cast<intptr_t>(wmInfo.info.win.hdc);

    createInfo.pixelFormatAttributes = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0,        //End
    };

    createInfo.attributesList = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        0
    };
#endif
    context = IGraphicsContext::createGraphicsContext(createInfo);
    if (context == nullptr)
    {
        TRACE(error) << "Failed to create graphics context";
        return false;
    }
    if (!context->makeCurrent())
    {
        TRACE(error) << "Failed to make context current";
    }
    if (!CLManager::create())
    {
        TRACE(error) << "Failed to initialize OpenCL manager";
        return false;
    }
    if (!CLManager::instance()->initializeWithGraphicsContext(context))
    {
        TRACE(error) << "Failed to initialize OpenCL with OpenGL context";
        return false;
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

    return true;
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
    if (currentTest != testList.end())
    {
        (*currentTest)->shutdown();
    }
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
    case SDLK_q:
        SDL_Event event;
        event.type = SDL_QUIT;
        SDL_PushEvent(&event);
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

void TestApp::Impl::reshape(int w, int h)
{
    context->makeCurrent();
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->resize(w, h);
        }
    }
    context->swapBuffers();
}

void TestApp::Impl::display()
{
    static double   updatelapse = 0;
    static uint32_t frameCtr    = 0;
    frameCtr++;
    uint64_t t1 = SDL_GetPerformanceCounter();

    context->makeCurrent();
    if (app->d->currentTest != app->d->testList.end())
    {
        auto test   = (*(app->d->currentTest)).get();
        auto uitest = dynamic_cast<BaseUITest*>(test);
        if (uitest)
        {
            uitest->display();
        }
    }
    context->swapBuffers();
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

void TestApp::Impl::testEventHandler(const BaseTest* test, TestEvent evt,
                                     void* userData)
{
    assert(userData && "Expected to have userData set");
    TRACE(trace) << ">> Test Name: " << std::string(test ? test->name() :
        "nullptr") << "Event: " << to_string(evt);
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
            app->d->context->makeCurrent();
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
    TRACE(trace) << "<< Test Name: " << std::string(test ? test->name() :
        "nullptr") << "Event: " << to_string(evt);
}
