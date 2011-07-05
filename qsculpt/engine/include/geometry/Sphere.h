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
#ifndef GEOMETRY_SPHERE_H_
#define GEOMETRY_SPHERE_H_

#include "Point3D.h"

namespace geometry {
    class Sphere {
        static const Point3 DEFAULT_CENTER;
        static const float DEFAULT_RADIUS;
        
        Point3 _origin;
        float  _r;
        
    public:
        Sphere(const Point3 & origin = DEFAULT_CENTER, 
               float r = DEFAULT_RADIUS);
        
        Point3 origin() const;
    };
    
    
    Sphere::Sphere(const Point3 & origin, 
                    float r)
                   : _origin(origin),
                   _r(r)
    {
    }
    
    Point3 Sphere::origin() const {
        return _origin;
    }

};

#endif
