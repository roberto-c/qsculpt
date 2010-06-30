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
#include "StdAfx.h"
#include "Camera.h"
#include <QString>
#include <math.h>

Camera::Camera()
 :  m_colatitude(0.0),
    m_longitude(0.0)
{
}


Camera::~Camera()
{
}


void Camera::setPosition(const Point3& p)
{
    m_position = p;

    Point3 v = m_position - m_target;
    m_distanceFromTarget = v.norm();
    m_colatitude = acos(v.z()/m_distanceFromTarget);
    m_longitude = acos(v.x()/(m_distanceFromTarget * sin(m_colatitude)));
}

Point3 Camera::getPosition()
{
    return m_position;
}

void Camera::setTargetPoint(const Point3 & target)
{
    m_target = target;

    Point3 v = m_position - m_target;
    m_distanceFromTarget = v.norm();
    m_colatitude = acos(v.z()/m_distanceFromTarget);
    m_longitude = acos(v.x()/(m_distanceFromTarget * sin(m_colatitude)));
}

Point3 Camera::getTargetPoint()
{
    return m_target;
}

void Camera::setOrientationVector(const Point3& v)
{
    m_orientation = v;
}

Point3 Camera::getOrientationVector()
{
    return m_orientation;
}

void Camera::setLongitude(double longitude)
{
    m_longitude = longitude;

    double x = m_distanceFromTarget * cos(m_longitude) * sin(m_colatitude);
    double y = m_distanceFromTarget * sin(m_longitude) * sin(m_colatitude);
    double z = m_distanceFromTarget * cos(m_colatitude);

    m_position = m_target + Point3(x, y, z);
}

double Camera::getLongitude()
{
    return m_longitude;
}

void Camera::setColatitude(double colatitude)
{
    m_colatitude = colatitude;

    double x = m_distanceFromTarget * cos(m_longitude) * sin(m_colatitude);
    double y = m_distanceFromTarget * sin(m_longitude) * sin(m_colatitude);
    double z = m_distanceFromTarget * cos(m_colatitude);

    m_position = m_target + Point3(x, y, z);
}

double Camera::getColatitude()
{
    return m_colatitude;
}

double Camera::getDistanceFromTarget()
{
    return m_distanceFromTarget;
}

void Camera::setDistanceFromTarget(double distance)
{
    m_distanceFromTarget = distance;

    double x = m_distanceFromTarget * cos(m_longitude) * sin(m_colatitude);
    double y = m_distanceFromTarget * sin(m_longitude) * sin(m_colatitude);
    double z = m_distanceFromTarget * cos(m_colatitude);

    m_position = m_target + Point3(x, y, z);
}

QString Camera::toString() const
{
    QString str;
    
    //str ="Camera Position: " + m_position.toString() + "Camera Target: " + m_target.toString()
    //        + "Camera Orientation: " + m_orientation.toString();
    
    return str;
}

