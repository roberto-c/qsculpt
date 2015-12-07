/*
 *  test.cpp
 *  TestApp
 *
 *  Created by Roberto Cabral on 2/22/11.
 *  Copyright 2011-2013 Roberto Cabral. All rights reserved.
 *
 */
#include "stable.h"

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
//#include <omp.h>

#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/opencl/OpenCL.h>

#include "TestApp.h"

extern "C" int SDL_main(int argc, char** argv) {
    //qInstallMsgHandler(myMessageOutput);
    int errorCode = 0;
    try {
        
        TestApp app(argc, argv);
        
        app.run();

    } catch(core::GlException & e){
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
        errorCode = -1;
    } catch(std::runtime_error & e){
        std::cerr << "std::runtime_error: Unhandled exception. " << e.what() << std::endl;
        errorCode = -2;
    } catch(std::exception &e){
        std::cerr << "std::exception: Unhandled exception. " << e.what() << std::endl;
        errorCode = -3;
    } catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
        errorCode = -4;
    }
    return errorCode;
}
