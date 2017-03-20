/***************************************************************************
 *   Copyright (C) 2011 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/geometry/Sphere.h>

// static definitions
namespace geometry
{
const Point3 Sphere::DEFAULT_CENTER = Point3(0, 0, 0);
const float  Sphere::DEFAULT_RADIUS = 1.0f;

Sphere::Sphere(const Point3& origin, float r)
    : _origin(origin)
    , _r(r)
{
}

Point3 Sphere::center() const { return _origin; }

void Sphere::setCenter(const Point3& val) { _origin = val; }

float Sphere::radius() const { return _r; }

void Sphere::setRadius(float val) { _r = val; }
}
