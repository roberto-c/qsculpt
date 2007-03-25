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
#ifndef CAMERA_H
#define CAMERA_H

#include "icamera.h"

/**
Implements the ICamera interface

	@author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class Camera : public ICamera
{
public:
    Camera();

    virtual ~Camera();

    virtual void setPosition(const Point3D& p);

    virtual Point3D getPosition();
    
    virtual void setTargetPoint(const Point3D & target);
    
    virtual Point3D getTargetPoint();
    
    virtual void setOrientationVector(const Point3D& v);
    
    virtual Point3D getOrientationVector();

	virtual void setLongitude(double longitude);

	virtual double getLongitude();

	virtual void setColatitude(double colatitude);

	virtual double getColatitude();

	virtual void setDistanceFromTarget(double distance);

	virtual double getDistanceFromTarget();

    QString toString();

private:
    Point3D m_position;
    Point3D m_target;
    Point3D m_orientation;

	double m_colatitude;
	double m_longitude;
	double m_distanceFromTarget;
};

#endif
