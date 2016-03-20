//
//  SubdivisionTest.h
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_SubdivisionTest_h
#define qsculpt_SubdivisionTest_h

#include "BaseTest.h"
#include "PlastilinaCore/Document.h"

class SubdivisionTest : public BaseTest 
{
    struct Impl;
    std::unique_ptr<Impl> d_;
    
public:
    
    SubdivisionTest();
    
    virtual ~SubdivisionTest();
    
    virtual void setup();

    virtual void run();

    virtual void shutdown();
};

#endif
