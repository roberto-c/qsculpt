#pragma once

#include <string>
#include <functional>

enum class TestEvent {
    TE_UNKNOWN,
    TE_SETUP_PRE,
    TE_SETUP_POST,
    TE_RUN_PRE,
    TE_RUN_POST,
    TE_SHUTDOWN_PRE,
    TE_SHUTDOWN_POST,
    TE_RUN_FRAME_PRE,
    TE_RUN_FRAME_POST,
    TE_RUN_TEST_END
};

std::string to_string(TestEvent evt);

class BaseTest
{
public:
    typedef std::function<void(const BaseTest*, TestEvent, void*)> FNCALLBACK;

    BaseTest();

    BaseTest(const std::string & name);

    virtual ~BaseTest() {}

    void setup();

    void run();
    
    void shutdown();

    std::string name() const;

    void notify(TestEvent evt) const;

    void setNotifyCallback(FNCALLBACK callback, void* userData);

protected:
    virtual void doSetup() = 0;

    virtual void doRun() = 0;

    virtual void doShutdown() = 0;

private:
    std::string         _name;
    FNCALLBACK          _callback;
    void*               _userData;
};

class BaseUITest : public BaseTest
{
public:
    BaseUITest();

    BaseUITest(const std::string name);

    virtual ~BaseUITest() {}

    virtual void resize(int w, int h) = 0;

            void display();

    virtual void keyboard(int key, int x, int y) = 0;

    virtual void mouseClick(uint32_t button, uint32_t state, int x, int y) = 0;

    virtual void mouseMove(uint32_t state, int x, int y) = 0;

protected:
    virtual void doRenderFrame() = 0;

};