/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#ifndef ICAMERA_H
#define ICAMERA_H

#include "Point3D.h"

/**
 * Camera class. Control the postion and orientation of the camera.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class ICamera{
public:
    ICamera() {}

    virtual ~ICamera() {}
    
    virtual void setPosition(const Point3& p) = 0;

    virtual Point3 getPosition() = 0;
    
    virtual void setTargetPoint(const Point3 & target) = 0;
    
    virtual Point3 getTargetPoint() = 0;
    
    virtual void setOrientationVector(const Point3& v) = 0;
    
    virtual Point3 getOrientationVector() = 0;

    virtual void setLongitude(double longitude) = 0;

    virtual double getLongitude() = 0;

    virtual void setColatitude(double colatitude) = 0;

    virtual double getColatitude() = 0;

    virtual void setDistanceFromTarget(double distance) = 0;

    virtual double getDistanceFromTarget() = 0;
};

#endif
