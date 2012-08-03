/*
 *  untitled.cpp
 *  qsculpt
 *
 *  Created by Roberto Cabral on 2/22/11.
 *  Copyright 2011 plastilinaware. All rights reserved.
 *
 */
#define __CL_ENABLE_EXCEPTIONS

#include <GLUT/GLUT.h>
#include <OpenCL/opencl.h>
#include "cl.hpp"

#include <CoreEngine/Plastilina.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
//#include <omp.h>

#include "TestApp.h"

#include "CoreEngine/Vertex.h"
#include "CoreEngine/HEdge.h"
#include "CoreEngine/Face.h"
#include "CoreEngine/Point3D.h"
#include "CoreEngine/Octree.h"
#include "CoreEngine/Vector.h"
#include "CoreEngine/Scene.h"
#include "CoreEngine/SceneNode.h"
#include "CoreEngine/subdivision/Sphere.h"
#include "CoreEngine/subdivision/Box.h"
#include "CoreEngine/subdivision/Subdivision.h"
#include "CoreEngine/geometry/Sphere.h"
#include "CoreEngine/geometry/Ray.h"
#include "CoreEngine/FlatRenderer.h"
#include "CoreEngine/Camera.h"

#include "SceneNodeTest.h"
#include "DocumentModelTest.h"
#include "SubdivisionTest.h"

#include "CoreEngine/GlslShader.h"
#include "CoreEngine/GlslProgram.h"
#include "CoreEngine/Color.h"

#define NOINLINE __attribute__ ((noinline))

#define LOOP_LENGTH 1000000000
//#define LOOP_LENGTH 10
#define NUM_THREADS (1)

using namespace data;

#define DATA_SIZE (1024)
const char* g_KernelSource = "\n" \
"__kernel square( __global float * input, \n" \
"                __global float * output, \n" \
"                const unsigned int count) \n" \
"{ \n" \
"    int i = get_local_id(0); \n" \
"    if (i < count) { \n" \
"        output[i] = input[i]*input[i]; \n" \
"    } \n" \
"} \n";





void openclTest()
{
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    std::string value;
    
    std::string val = std::string("test");
    
    try {
        cl::Platform::get(&platforms);
        if (platforms.size() > 0) {
            platforms[0].getInfo(CL_PLATFORM_NAME, &value);
            std::cout << "Platform name: " << value << std::endl;
        }
    } catch (cl::Error & e) {
        std::cerr << "Error: " << e.what() << std::endl; 
    }

}

void printSceneNode(SceneNode::weak_ptr node, const std::string & indent = "") {
    if (node.expired())
        return;
    
    auto n = node.lock();
    std::cout << indent << n->name() << std::endl;
    std::string tmpStr = indent + std::string("   ");
    
    Iterator<SceneNode> it = n->constIterator();
    while (it.hasNext()) {
        auto n2 = it.next();
        printSceneNode(n2, tmpStr);
    }
}

void printScene(Scene::weak_ptr node, const std::string & indent = "") {
    if (node.expired())
        return;
    
    std::cout << indent << "root" << std::endl;
    std::string tmpStr = indent + "   ";
    
    auto n = node.lock();
    Iterator<SceneNode> it = n->constIterator();
    while (it.hasNext()) {
        auto n2 = it.next();
        printSceneNode(n2, tmpStr);
    }
}

bool sceneTest() {
    Scene::shared_ptr scene = std::make_shared<Scene>();
    
    SceneNode::shared_ptr node1(new SurfaceNode(new Sphere));
    node1->setName("Node 1");
    
    SceneNode::shared_ptr node2(new SurfaceNode(new Sphere));
    node2->setParent(node1);
    node2->setName("Node 2");
    
    node1->add(node1);
    scene->add(node1);
    
    printScene(scene);
    
    return true;
}

bool surfaceTest() {
    std::unique_ptr<ISurface> surf(new Subdivision);
    
    Point3 vtx, tmp;
    std::vector<size_t> vertexID;
    
    vertexID.push_back(surf->addVertex(Point3(0,0,0)));
    vertexID.push_back(surf->addVertex(Point3(1,0,0)));
    vertexID.push_back(surf->addVertex(Point3(0,1,0)));
    vertexID.push_back(surf->addVertex(Point3(0,0,1)));
    
    Iterator<Vertex> it = surf->constVertexIterator();
    while(it.hasNext()) {
        std::cerr << toString(*it.peekNext());
        it.next();
    }
    
    return true;
}


bool vectorTest() {
    std::cerr << toString(Point3());
    
    return true;
}

void sphereIntersectionTest() 
{
    geometry::Sphere sphere;
    geometry::Ray ray;
    Point3 p;
    
    ray.origin() = Point3(-1.5, 0.999, 0);
    
    std::cerr << "Ray: origin = " << toString(ray.origin()) << " dir = " << toString(ray.direction());
    std::cerr << "Sphere: center = " << toString(sphere.center()) << " radius = " << sphere.radius();
    float t = ray.intersect(sphere, &p);
    std::cerr << "t = " << t;
    if ( t >= 0 ) {
        std::cerr << toString(p);
    } else {
        
    }
}

bool faceIteratorVertexTest() 
{
    try {        
        std::unique_ptr<ISurface> surf(new Subdivision);
        
        Point3 vtx, tmp;
        std::vector<size_t> vertexID;
        Vertex::shared_ptr v;
        
        vertexID.push_back(surf->addVertex(Point3(0,0,0)));
        vertexID.push_back(surf->addVertex(Point3(1,0,0)));
        vertexID.push_back(surf->addVertex(Point3(0,1,0)));
        vertexID.push_back(surf->addVertex(Point3(0,0,1)));
        
        std::cerr << "new face IID:" << surf->addFace(vertexID);
        
        Iterator<Vertex> it = surf->constVertexIterator();
        while(it.hasNext()) {
            std::cerr << toString(*it.peekNext());
            v = it.next();
        }

        Iterator<Face> faceIt = surf->constFaceIterator();
        while(faceIt.hasNext()) {
            std::cerr << "Face IID: " << faceIt.peekNext()->iid();
            faceIt.next();
        }

        std::cerr << "v.edge() " << v->edge()->iid();
        std::cerr << "v.edge().face() " << (v->edge()->face() ? v->edge()->face()->iid() : -1);
        std::cerr << "v.edge().pair() " << (v->edge()->pair() && v->edge()->pair() ? v->edge()->pair()->iid() : -1);
        if (v->edge()->pair()) {
            std::cerr << "v.edge().pair().face() " << (v->edge()->pair()->face() ? v->edge()->pair()->face()->iid() : -1);
        }
            
        
//        faceIt = v->faceIterator();
//        while(faceIt.hasNext()) {
//            std::cerr << "Vertex: Face IID: " << faceIt.next().iid();
//        }
     
    } catch (std::runtime_error & e) {
        std::cerr << e.what();
    }
    return true;
}

struct MyFunctor {
    MyFunctor() { std::cerr << "MyFunctor constructor"; }
    ~MyFunctor() { std::cerr << "MyFunctor destructor"; }
    int operator()(int b) {
        return b+5;
    }
};

template<typename T, typename Functor>
class FunctorTest {
public:
    void doSomething(int a) {
        
    }
};

void iteratorTest(); 

extern "C" int SDL_main(int argc, char** argv) {
    //qInstallMsgHandler(myMessageOutput);
    
    try {
        
        TestApp app(argc, argv);
        
        app.run();

    } catch(core::GlException & e){
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    } catch(std::runtime_error & e){
        std::cerr << "std::runtime_error: Unhandled exception. " << e.what() << std::endl;
    } catch(std::exception &e){
        std::cerr << "std::exception: Unhandled exception. " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }
    return 0;
}
