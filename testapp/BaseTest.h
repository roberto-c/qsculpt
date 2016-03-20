#pragma once

#include <string>

class BaseTest
{
public:
    BaseTest();

    BaseTest(const std::string & name);

    virtual ~BaseTest() {}

    virtual void setup() = 0;

    virtual void run() = 0;

    virtual void shutdown() = 0;

    std::string name() const;

private:
    std::string     _name;
};

class BaseWindowTest : public BaseTest
{
public:
    BaseWindowTest();

    BaseWindowTest(const std::string name);

    virtual ~BaseWindowTest() {}

    virtual void resize(int w, int h) = 0;

    virtual void display() = 0;
};