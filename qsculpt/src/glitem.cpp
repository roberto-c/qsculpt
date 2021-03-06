/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
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
#include "Stable.h"
#include "glitem.h"
#include "GlView.h"

#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/subdivision/Box.h>
#include "DocumentView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

namespace Plastilina
{

GlItem::GlItem()
    : _s(new Box)
{
}

GlItem::~GlItem() {}

void GlItem::paintGl(GlCanvas* c)
{
    assert(c != NULL);

    // c->drawLine();
}
}