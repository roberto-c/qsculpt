//
//  DocumentModelTest.h
//  qsculpt
//
//  Created by Juan Cabral on 6/5/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_DocumentModelTest_h
#define qsculpt_DocumentModelTest_h

#include "PlastilinaCore/Document.h"

class DocumentModelTest
{
    Document::shared_ptr doc;

    SurfaceNode::shared_ptr node1;
    SurfaceNode::shared_ptr node2;
    SurfaceNode::shared_ptr node3;

    int setup();

    int test();

    int cleanup();

  public:
    int run();
};

#endif
