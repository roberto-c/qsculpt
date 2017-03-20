/***************************************************************************
 *   Copyright (C) 2014 by Juan R Cabral                                   *
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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/Context_private.h>
#include <PlastilinaCore/Plastilina.h>

namespace core
{
Context::Context(const CtxAttributeList& attributes)
    : d(new Impl())
{
    if (d)
    {
        d->createGlCtx(attributes);
        d->createClCtx(attributes);
        d->createVkCtx(attributes);
    }
}

Context::~Context() {}

CtxAttribute Context::attribute(CtxAttribute name)
{
    CtxAttribute value = CTX_ATR_NULL;
    if (d->attribute(name, &value))
        return value;

    return core::CTX_ATR_NULL;
}

void Context::setAttribute(CtxAttribute name, CtxAttribute value)
{
    // Process the attribute by platform
    d->setAttribute(name, value);

    // Now do common things
    switch (name)
    {
    case CTX_ATR_GL_DOUBLE_BUFFER:

        break;

    default:
        break;
    }
}
}