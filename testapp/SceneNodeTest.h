//
//  SceneNodeTest.h
//  qsculpt
//
//  Created by Juan Cabral on 12/3/11.
//  Copyright (c) 2011 plastilinaware. All rights reserved.
//

#ifndef qsculpt_SceneNodeTest_h
#define qsculpt_SceneNodeTest_h

#include "PlastilinaCore/Scene.h"
#include "PlastilinaCore/SceneNode.h"
#include "PlastilinaCore/subdivision/Subdivision.h"
#include "PlastilinaCore/Camera.h"

class SubdivisionClosestPointTest {
    Camera camera;
    Scene::shared_ptr scene;
    SurfaceNode::shared_ptr node1;
    SurfaceNode::shared_ptr node2;
    
    int setup();
    
    int test();
    
    int cleanup();
    
    void printSceneNode(Scene::shared_ptr node, const std::string & indent = "");
    
    void printSceneNode(SceneNode::shared_ptr node, const std::string & indent = "");
    
public:
    int run();
};

int SubdivisionClosestPointTest::setup() {
    scene = std::make_shared<Scene>();
    
    ISurface * surf = new Subdivision();
    surf->addVertex(Point3(0,0,0));
    surf->addVertex(Point3(1,0,0));
    
    node1 = std::make_shared<SurfaceNode>(surf);
    node1->transform().translate(Vector3(1, 0, 0));
    node1->setName("Node 1");
    
    surf = new Subdivision();
    surf->addVertex(Point3(0,0,0));
    surf->addVertex(Point3(1,0,0));
    
    node2 = std::make_shared<SurfaceNode>(surf);
    node2->setParent(node1);
    node1->add(node2);
    node2->setName("Node 2");
    node2->transform().rotate(Eigen::AngleAxisf(M_PI_4, Vector3(0.f, 0.f, 1.f)));
    //node2->transform().translate(Vector3(1, 0, 0));
    scene->add(node1);
    
    return 0;
}

int SubdivisionClosestPointTest::test() {
    // Print points in nodes:
    printSceneNode(scene);
    
    Point3 p = Point3(3.8f,0,0);
    std::cerr << "Search for : " << toString(p);
    Point3 p2 = node1->worldToLocal(p);
    std::cerr << "P2 : " << toString(p2);
    size_t vtxiid = node1->surface()->getClosestPointAtPoint(p2);
    std::cerr << "Node1 Vertex IID: " << vtxiid;
    Point3 p3 = node2->worldToLocal(p);
    std::cerr << "P3 : " << toString(p3);
    std::cerr << "Node2 Vertex IID: " << node2->surface()->getClosestPointAtPoint(p3);
    return 0;
}

int SubdivisionClosestPointTest::cleanup() {
    return 0;
}

int SubdivisionClosestPointTest::run() {
    if (setup() != 0) {
        return -1;
    }
    int err = test();
    cleanup();
    
    return err;
}

void SubdivisionClosestPointTest::printSceneNode(SceneNode::shared_ptr node, const std::string & indent) {
    if (node)
        return;
    
    std::string tmpStr = indent + std::string("   ");
    std::cout << indent << node->name() << std::endl;
    
    if (node->nodeType() == NT_Surface) {
        SurfaceNode::shared_ptr surf = std::static_pointer_cast<SurfaceNode>(node);
        Iterator<Vertex> vtxIt = surf->surface()->constVertexIterator();
        while (vtxIt.hasNext()) {
            auto v = vtxIt.next();
            Point3 p = v->position();
            p = node->parentTransform() * node->transform() * p;
            std::cout << indent << "Vertex: " << v->iid() << " " << toString(p) << std::endl;
        }
    }
    
    Iterator<SceneNode> it = node->constIterator();
    while (it.hasNext()) {
        auto n = it.next();
        printSceneNode(n, tmpStr);
    }
}

void SubdivisionClosestPointTest::printSceneNode(Scene::shared_ptr node, const std::string & indent) {
    if (node)
        return;
    
    std::cout << indent << "root" << std::endl;
    std::string tmpStr = indent + "   ";
    
    Iterator<SceneNode> it = node->constIterator();
    while (it.hasNext()) {
        auto n = it.next();
        printSceneNode(n, tmpStr);
    }
}

#endif
