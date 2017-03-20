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

class SubdivisionTest : public BaseUITest
{
    struct Impl;
    std::unique_ptr<Impl> d_;

  public:
    SubdivisionTest();

    virtual ~SubdivisionTest();

    virtual void resize(int w, int h);

    virtual void keyboard(int key, int x, int y);

    virtual void mouseClick(uint32_t button, uint32_t state, int x, int y);

    virtual void mouseMove(uint32_t state, int x, int y);

  protected:
    virtual void doSetup();

    virtual void doRun();

    virtual void doShutdown();

    virtual void doRenderFrame();
};

#endif
