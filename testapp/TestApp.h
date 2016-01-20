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
    struct Impl;
    std::unique_ptr<Impl> d;
    
public:
    
    TestApp(int argc, char** argv) ;
    
    ~TestApp();
    
    void init(int argc, char** argv);
    
    int run() ;
};

#endif
