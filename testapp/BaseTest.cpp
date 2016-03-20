#include "BaseTest.h"

BaseTest::BaseTest()
{
}

BaseTest::BaseTest(const std::string & name)
    : _name(name)
{
}

std::string BaseTest::name() const
{
    return _name;
}

BaseWindowTest::BaseWindowTest()
    : BaseTest()
{
}

BaseWindowTest::BaseWindowTest(const std::string name)
    : BaseTest(name)
{
}
