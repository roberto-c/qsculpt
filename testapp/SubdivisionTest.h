//
//  SubdivisionTest.h
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_SubdivisionTest_h
#define qsculpt_SubdivisionTest_h

#include "PlastilinaCore/Document.h"

class SubdivisionTest {
    struct Impl;
    std::unique_ptr<Impl> d_;
    
public:
    
    SubdivisionTest();
    
    virtual ~SubdivisionTest();
    
    int run();
};

#endif
