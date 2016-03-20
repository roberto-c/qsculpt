//
//  SubdivisionTest.h
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#pragma once

#include "stable.h"
#include "BaseTest.h"
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Document.h>

class CameraTest : public BaseTest 
{
    
public:
    
    CameraTest();
    
    virtual ~CameraTest();
    
    virtual void setup();

    virtual void run();

    virtual void shutdown();
};

inline CameraTest::CameraTest()
    : BaseTest("CameraTest")
{
}

inline CameraTest::~CameraTest()
{
}

inline void CameraTest::setup()
{
    TRACE(info) << "Seup";
}

inline void CameraTest::run() 
{
    TRACE(info) << "run";
    Camera camera;
    camera.setOrthoMatrix(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    camera.setViewport(0, 0, 640, 480);
    Vector4 p4(0, 0, 0, 1);
    Vector3 p3(0, 0, 0);

    auto r = camera.localToWorld(p3);
    TRACE(info) << "Point: " << core::utils::to_string(p3) << " localToWorld: " << core::utils::to_string(r);
}

inline void CameraTest::shutdown()
{
    TRACE(info) << "shutdown";
}
