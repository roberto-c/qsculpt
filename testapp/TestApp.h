//
//  TestApp.h
//  qsculpt
//
//  Created by Juan Cabral on 7/1/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_TestApp_h
#define qsculpt_TestApp_h

#include <SDL2/SDL.h>

class TestApp
{
public:
    TestApp(int argc, char** argv);

    ~TestApp();

    int run();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

};

#endif
