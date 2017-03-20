//
//  DocumentModelTest.cpp
//  qsculpt
//
//  Created by Juan Cabral on 6/6/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "Stable.h"
#include <iostream>

#include <PlastilinaCore/Document.h>
#include "PlastilinaCore/ISurface.h"
#include "PlastilinaCore/Subdivision/Subdivision.h"

#include "DocumentModelTest.h"
#include "PlastilinaCore/Logging.h"

static void printModelRecursive(
    const Document::shared_ptr&  doc,
    const SceneNode::shared_ptr& parent = SceneNode::shared_ptr())
{
    TRACEFUNCTION("");

    size_t numrows = doc->childrenCount(parent);
    for (unsigned int i = 0; i < numrows; ++i)
    {
        auto index = doc->index(i, parent);
        std::cerr << "iid: " << index->iid() << std::endl;
        printModelRecursive(doc, index);
    }
}

int DocumentModelTest::setup()
{
    TRACEFUNCTION("");
    doc = std::make_shared<Document>();

    ISurface* surf = new Subdivision();
    // surf->addVertex(Point3(0,0,0));
    // surf->addVertex(Point3(1,0,0));

    node1 = std::make_shared<SurfaceNode>(surf);
    node1->transform().translate(Vector3(1, 0, 0));
    node1->setName("Node 1");

    surf = new Subdivision();
    // surf->addVertex(Point3(0,0,0));
    // surf->addVertex(Point3(1,0,0));

    node2 = std::make_shared<SurfaceNode>(surf);
    node2->setName("Node 2");
    node2->transform().rotate(
        Eigen::AngleAxisf(float(M_PI_4), Vector3(0.f, 0.f, 1.f)));

    node3 = std::make_shared<SurfaceNode>(surf);
    node3->setName("Node 3");

    doc->addItem(node1);
    auto index = doc->index(0);
    if (index)
    {
        std::cerr << "Index is valid." << std::endl;
        doc->addItem(node2, index);
        doc->addItem(node3, index);
    }
    else
    {
        std::cerr << "Index is invalid." << std::endl;
    }
    return 0;
}

int DocumentModelTest::test()
{
    TRACEFUNCTION("");
    // Print points in nodes:
    auto scene = doc->scene().lock();
    //    printSceneNode(scene);

    auto ptr = doc->findItem(node3->iid());
    std::cerr << "node3: " << node3->iid();
    std::cerr << "ptr: " << (ptr ? ptr->iid() : 0);

    auto index1 = doc->index(0);
    auto index2 = doc->index(0, index1);
    auto index3 = doc->index(1, index1);
    std::cerr << "index1: " << index1->iid();
    std::cerr << "index2: " << index2->iid();
    std::cerr << "index3: " << index3->iid();

    auto ptr2   = doc->findItem(index2->iid());
    auto parent = ptr2->parent().lock();
    std::cerr << "Parent: " << parent->iid();

    printModelRecursive(doc);
    return 0;
}

int DocumentModelTest::cleanup()
{
    TRACEFUNCTION("");
    return 0;
}

int DocumentModelTest::run()
{
    TRACEFUNCTION("");
    if (setup() != 0)
    {
        return -1;
    }
    int err = test();
    cleanup();

    return err;
}
