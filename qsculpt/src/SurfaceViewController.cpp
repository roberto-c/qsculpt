/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores   *
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
#include "SurfaceViewController.h"
#include <PlastilinaCore/ISurface.h>

struct SurfaceViewController::PrivateData
{
    ISurface* surface;
    // EditMode                mode;
    std::vector<VertexHandle*> vertices;
};

SurfaceViewController::SurfaceViewController(ISurface* surface)
{
    Q_UNUSED(surface);
}

SurfaceViewController::~SurfaceViewController() {}

void SurfaceViewController::paintGL() {}
