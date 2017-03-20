#include "Stable.h"
#include "BaseTest.h"

std::string to_string(TestEvent evt)
{
#define ENUM(X)                                                              \
    case X:                                                                  \
        return std::string(#X)
    switch (evt)
    {
        ENUM(TestEvent::TE_UNKNOWN);
        ENUM(TestEvent::TE_SETUP_PRE);
        ENUM(TestEvent::TE_SETUP_POST);
        ENUM(TestEvent::TE_RUN_PRE);
        ENUM(TestEvent::TE_RUN_POST);
        ENUM(TestEvent::TE_RUN_FRAME_PRE);
        ENUM(TestEvent::TE_RUN_FRAME_POST);
        ENUM(TestEvent::TE_SHUTDOWN_PRE);
        ENUM(TestEvent::TE_SHUTDOWN_POST);
    default:
        return "BaseTest::TestEvent::<UNKNOWN>";
    }
#undef ENUM
}

BaseTest::BaseTest()
    : _callback(nullptr)
    , _userData(nullptr)
{
}

BaseTest::BaseTest(const std::string& name)
    : _name(name)
    , _callback(nullptr)
    , _userData(nullptr)
{
}

void BaseTest::setup()
{
    notify(TestEvent::TE_SETUP_PRE);
    doSetup();
    notify(TestEvent::TE_SETUP_POST);
}

void BaseTest::run()
{
    notify(TestEvent::TE_RUN_PRE);
    doRun();
    notify(TestEvent::TE_RUN_POST);
}

void BaseTest::shutdown()
{
    notify(TestEvent::TE_SHUTDOWN_PRE);
    doShutdown();
    notify(TestEvent::TE_SHUTDOWN_POST);
}

std::string BaseTest::name() const { return _name; }

void BaseTest::notify(TestEvent evt) const
{
    if (_callback)
    {
        _callback(this, evt, _userData);
    }
}

void BaseTest::setNotifyCallback(BaseTest::FNCALLBACK callback,
                                 void*                userData)
{
    _callback = callback;
    _userData = userData;
}

BaseUITest::BaseUITest()
    : BaseTest()
{
}

BaseUITest::BaseUITest(const std::string name)
    : BaseTest(name)
{
}

void BaseUITest::display()
{
    notify(TestEvent::TE_RUN_FRAME_PRE);
    doRenderFrame();
    notify(TestEvent::TE_RUN_FRAME_POST);
}
