/***************************************************************************
 *   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#pragma once

#include "BaseTest.h"
#include "PlastilinaCore/Document.h"

class CanvasTest : public BaseUITest
{
    struct Impl;
    std::unique_ptr<Impl> d_;

  public:
    CanvasTest();

    virtual ~CanvasTest();

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