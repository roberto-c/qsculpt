//
//  SubdivisionTest.h
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_SubdivisionTest_h
#define qsculpt_SubdivisionTest_h

#include "CoreEngine/Document.h"

class SubdivisionTest {
    Document::shared_ptr    doc;
    
    SurfaceNode::shared_ptr node1;
    
    int setup();
    
    int test();
    
    int cleanup();
    
public:
    int run();
};

#endif
