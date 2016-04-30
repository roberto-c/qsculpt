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
    
protected:
    virtual void doSetup();

    virtual void doRun();

    virtual void doShutdown();

private:
    Eigen::IOFormat 					octaveFmt;
};

inline CameraTest::CameraTest()
    : BaseTest("CameraTest")
    , octaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]")
{
}

inline CameraTest::~CameraTest()
{
}

inline void CameraTest::doSetup()
{

}

inline void CameraTest::doRun()
{
    TRACE(info) << "run";

    {
        Camera camera;
        camera.setOrthoMatrix(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
        camera.setViewport(0, 0, 640, 480);
        Vector4 p4(0, 0, 0, 1);
        Vector3 p3(0, 0, 0);
        Vector3 res(320.0f, 240.0f, 0.5f);

        TRACE(trace) << "Viewport: " << camera.viewport().format(octaveFmt);
        TRACE(trace) << "Projection: " << camera.projection().format(octaveFmt);
        TRACE(trace) << "View matrix: " << camera.modelView().format(octaveFmt);
        auto r = camera.localToWorld(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " localToWorld: " << core::utils::to_string(r);
        TRACE(info) << "Expected EQUAL:  " << std::string(p3 == r ? "PASSED" : "FAILED");
        r = camera.worldToEye(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " worldToEye: " << core::utils::to_string(r);
        TRACE(info) << "Expected value= " << core::utils::to_string(res) << " " << std::string(r == res ? "PASSED" : "FAILED");
    }

    {
        Camera camera;
        camera.setOrthoMatrix(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
        camera.setViewport(0, 0, 640, 480);

        Vector3 p3(10, 0, 0);
        Vector3 res(640.0f, 240.0f, 0.5f);

        TRACE(trace) << "Viewport: " << camera.viewport().format(octaveFmt);
        TRACE(trace) << "Projection: " << camera.projection().format(octaveFmt);
        TRACE(trace) << "View matrix: " << camera.modelView().format(octaveFmt);
        auto r = camera.worldToEye(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " worldToEye: " << core::utils::to_string(r);
        TRACE(info) << "Expected value:  " << core::utils::to_string(Vector3(10, 0, 0)) << " " << std::string(Vector3(10, 0, 0) == r ? "PASSED" : "FAILED");

        p3 = Vector3(10.0f, 0.0f, 0.0f);
        res = Vector3(640.0f, 240.0f, 0.5f);
        r = camera.worldToScreen(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " worldToScreen: " << core::utils::to_string(r);
        TRACE(info) << "Expected value= " << core::utils::to_string(res) << " " << std::string(r == res ? "PASSED" : "FAILED");

        p3 = Vector3(640.0f, 240.0f, 0.5f);
        res = Vector3(10.0f, 0.0f, 0.0f);
        r = camera.screenToWorld(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " screenToWorld: " << core::utils::to_string(r);
        TRACE(info) << "Expected value= " << core::utils::to_string(res) << " " << std::string(r == res ? "PASSED" : "FAILED");
    }

    {
        Camera camera;
        camera.setOrthoMatrix(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
        camera.setViewport(0, 0, 640, 480);
        camera.transform() = Eigen::Translation3f(-1.0f, 1.0f, 0.0f);

        Vector3 p3(10, 0, 0);
        Vector3 res(640.0f, 240.0f, 0.5f);

        TRACE(trace) << "Viewport: " << camera.viewport().format(octaveFmt);
        TRACE(trace) << "Projection: " << camera.projection().format(octaveFmt);
        TRACE(trace) << "View matrix: " << camera.modelView().format(octaveFmt);
        auto r = camera.worldToEye(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " worldToEye: " << core::utils::to_string(r);
        TRACE(info) << "Expected value:  " << core::utils::to_string(Vector3(11, -1, 0)) << " " << std::string(Vector3(11, -1, 0) == r ? "PASSED" : "FAILED");

        p3 = Vector3(10.0f, 0.0f, 0.0f);
        res = Vector3(640.0f, 240.0f, 0.5f);
        r = camera.worldToScreen(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " worldToScreen: " << core::utils::to_string(r);
        TRACE(info) << "Expected value= " << core::utils::to_string(res) << " " << std::string(r == res ? "PASSED" : "FAILED");
        
        p3 = Vector3(640.0f, 240.0f, 0.5f);
        res = Vector3(10.0f, 0.0f, 0.0f);
        r = camera.screenToWorld(p3);
        TRACE(info) << "Point: " << core::utils::to_string(p3) << " screenToWorld: " << core::utils::to_string(r);
        TRACE(info) << "Expected value= " << core::utils::to_string(res) << " " << std::string(r == res ? "PASSED" : "FAILED");
    }
}

/*
*/


inline void CameraTest::doShutdown()
{
    TRACE(info) << "shutdown";
}
