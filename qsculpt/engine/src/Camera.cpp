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
#include "Point3D.h"
#include "Camera.h"
#include <QString>
#include <math.h>
#include <iostream>


#define WINDOW_NEAR (0.0f)
#define WINDOW_FAR  (1.0f)

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

    updateViewMatrix();
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

    updateViewMatrix();
}

Point3 Camera::getTargetPoint()
{
    return m_target;
}

void Camera::setOrientationVector(const Point3& v)
{
    m_orientation = v;

    updateViewMatrix();
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

    updateViewMatrix();
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

    updateViewMatrix();
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

    updateViewMatrix();
}

QString Camera::toString() const
{
    QString str;
    
    str ="Camera Position: " + ::toString(m_position) + "Camera Target: " + ::toString(m_target)
         + "Camera Orientation: " + ::toString(m_orientation);
    
    return str;
}

void Camera::setModelView(const Eigen::Matrix4f &m)
{
    _viewMat = m;
}

const Eigen::Matrix4f& Camera::modelView()
{
    return _viewMat;
}

void Camera::updateViewMatrix()
{
    Vector3 w = - (m_target - m_position).normalized();
    Vector3 u = m_orientation.cross(w).normalized();
    Vector3 v = w.cross(u);

    _viewMat.setIdentity();
    _viewMat(0, 0) = u.x();
    _viewMat(0, 1) = u.y();
    _viewMat(0, 2) = u.z();

    _viewMat(1, 0) = v.x();
    _viewMat(1, 1) = v.y();
    _viewMat(1, 2) = v.z();

    _viewMat(2, 0) = w.x();
    _viewMat(2, 1) = w.y();
    _viewMat(2, 2) = w.z();

    Eigen::Transform3f t;
    t.matrix() = _viewMat;
    t.translate(-m_position);
    _viewMat = t.matrix();
}

void Camera::setProjectionMatrix(const Eigen::Matrix4f &m)
{
    _projMat = m;
}

const Eigen::Matrix4f& Camera::projection()
{
    return _projMat;
}

void Camera::setPerspectiveMatrix(float left, float right,
                                  float bottom, float top,
                                  float near, float far)
{
    float dLR = right - left;
    float dBT = top - bottom;
    float dNF = far - near;

    assert (dLR != 0 && dBT != 0 && dNF != 0);

    float A = (right + left) / dLR;
    float B = (top + bottom) / dBT;
    float C = - (far + near) / dNF;
    float D = - (2 * far * near) / dNF;

    _projMat.setZero();
    _projMat(0, 0) =  ( 2 * near) / dLR;
    _projMat(0, 2) = A;
    _projMat(1, 1) =  ( 2 * near) / dBT;
    _projMat(1, 2) = B;
    _projMat(2, 2) = C;
    _projMat(2, 3) = D;
    _projMat(3, 2) = -1;
    _projMat(3, 3) = 0;
}

void Camera::setOrthoMatrix(float left, float right,
                                  float bottom, float top,
                                  float near, float far)
{
    float dLR = right - left;
    float dBT = top - bottom;
    float dNF = far - near;

    assert (dLR != 0 && dBT != 0 && dNF != 0);

    float tx = - (right + left) / dLR;
    float ty = - (top + bottom) / dBT;
    float tz = - (far + near) / dNF;

    _projMat.setIdentity();
    _projMat(0, 0) =  2 / dLR;
    _projMat(1, 1) =  2 / dBT;
    _projMat(2, 2) =  - 2 / dNF;
    _projMat(0, 3) = tx;
    _projMat(1, 3) = ty;
    _projMat(2, 3) = tz;
}

void Camera::setViewport(const Eigen::Matrix4f &m)
{
    _viewportMat = m;
}


void Camera::setViewport(int x, int y, int w, int h)
{
    float n = WINDOW_NEAR, f = WINDOW_FAR;
    _viewportMat.setIdentity();
    _viewportMat(0, 0) = w/2;
    _viewportMat(1, 1) = h/2;
    _viewportMat(0, 3) = w / 2 + x;
    _viewportMat(1, 3) = h / 2 + y;
    _viewportMat(2, 2) = (f-n)/2;
    _viewportMat(2, 3) = (f+n)/2;
}

const Eigen::Matrix4f& Camera::viewport() const
{
    return _viewportMat;
}

Vector3 Camera::eyeToWorld(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    o = _viewMat.inverse() * _projMat.inverse() * _viewportMat.inverse() * p4;

    return Vector3(o.data());
}

Vector3 Camera::worldToEye(const Vector3 &p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    o = _viewportMat * _projMat * _viewMat * p4;
    Vector3 t(o.data());
    return t;
}
