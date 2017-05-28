/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores   *
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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/FlatRenderer.h>
#include <PlastilinaCore/PointRenderer.h>
#include <PlastilinaCore/RendererFactory.h>
#include <PlastilinaCore/SmoothRenderer.h>
#include <PlastilinaCore/WireframeRenderer.h>

RendererFactory::RendererFactory() {}

RendererFactory::~RendererFactory() {}

IRenderer* RendererFactory::getRenderer(DrawingMode drawingMode)
{
    switch (drawingMode)
    {
    case DrawingMode::Points:
        return new PointRenderer;
        break;

    case DrawingMode::Wireframe:
        return new WireframeRenderer;
        break;

    case DrawingMode::Flat:
        return new FlatRenderer;
        break;

    case DrawingMode::Smooth:
        return new SmoothRenderer;
        break;

    case DrawingMode::Texture:
    default:
        return new FlatRenderer;
    }
}