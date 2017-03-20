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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/Point3D.h>
#include <math.h>
#include <iostream>

#define WINDOW_NEAR (0.0f)
#define WINDOW_FAR (1.0f)

Camera::Camera()
    : CameraNode()
{
}

Camera::Camera(const Camera& orig)
    : _projMat(orig._projMat)
    , _viewportMat(orig._viewportMat)
{
}

Camera& Camera::operator=(const Camera& rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    _projMat     = rhs._projMat;
    _viewportMat = rhs._viewportMat;

    return *this;
}

Camera::~Camera() {}

std::string Camera::toString() const
{
    std::string str;

    return str;
}

void Camera::setModelView(const Eigen::Matrix4f& m)
{
    transform().matrix() = m;
}

const Eigen::Matrix4f& Camera::modelView() { return transform().matrix(); }

void Camera::setProjectionMatrix(const Eigen::Matrix4f& m) { _projMat = m; }

const Eigen::Matrix4f& Camera::projection() { return _projMat; }

void Camera::setPerspectiveMatrix(float left, float right, float bottom,
                                  float top, float near_plane,
                                  float far_plane)
{
    float dLR = right - left;
    float dBT = top - bottom;
    float dNF = far_plane - near_plane;

    assert(dLR != 0 && dBT != 0 && dNF != 0);

    float A = (right + left) / dLR;
    float B = (top + bottom) / dBT;
    float C = -(far_plane + near_plane) / dNF;
    float D = -(2 * far_plane * near_plane) / dNF;

    _projMat.setZero();
    _projMat(0, 0) = (2.0f * near_plane) / dLR;
    _projMat(0, 2) = A;
    _projMat(1, 1) = (2.0f * near_plane) / dBT;
    _projMat(1, 2) = B;
    _projMat(2, 2) = C;
    _projMat(2, 3) = D;
    _projMat(3, 2) = -1;
    _projMat(3, 3) = 0;
}

void Camera::setOrthoMatrix(float left, float right, float bottom, float top,
                            float near_plane, float far_plane)
{
    float dLR = right - left;
    float dBT = top - bottom;
    float dNF = far_plane - near_plane;

    assert(dLR != 0 && dBT != 0 && dNF != 0);

    float tx = -(right + left) / dLR;
    float ty = -(top + bottom) / dBT;
    float tz = -(far_plane + near_plane) / dNF;

    _projMat.setIdentity();
    _projMat(0, 0) = 2.0f / dLR;
    _projMat(1, 1) = 2.0f / dBT;
    _projMat(2, 2) = -2.0f / dNF;
    _projMat(0, 3) = tx;
    _projMat(1, 3) = ty;
    _projMat(2, 3) = tz;
}

void Camera::setPerspectiveMatrix(float fovy, float aspect, float zNear,
                                  float zFar)
{
    assert(zNear > 0 && zFar > 0 && fovy > 0 && fovy < 180.0);

    fovy    = fovy / 2.0f;
    float t = tanf(fovy * float(M_PI) / 180.0f) * zNear;
    float b = -t;
    float r = t * aspect;
    float l = -r;
    setPerspectiveMatrix(l, r, b, t, zNear, zFar);
}

void Camera::setViewport(const Eigen::Matrix4f& m)
{
    _viewport = Vector4(m(0, 0) - m(0, 3), m(1, 1) - m(1, 3), m(0, 0) * 2,
                        m(1, 1) * 2);
    _viewportMat = m;
}

void Camera::setViewport(int x, int y, int w, int h)
{
    float n = WINDOW_NEAR, f = WINDOW_FAR;
    _viewport = Vector4(x, y, w, h);
    _viewportMat.setZero();
    _viewportMat(0, 0) = w / 2.f;
    _viewportMat(1, 1) = h / 2.f;
    _viewportMat(0, 3) = w / 2.f + x;
    _viewportMat(1, 3) = h / 2.f + y;
    _viewportMat(2, 2) = (f - n) / 2.f;
    _viewportMat(2, 3) = (f + n) / 2.f;
}

const Eigen::Matrix4f& Camera::viewport() const { return _viewportMat; }

Vector3 Camera::eyeToWorld(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    // o = p4 * _viewMat.inverse() * _projMat.inverse();
    auto m = transform().matrix();
    p4     = m.inverse() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::eyeToWorld(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    // o = p4 * _viewMat.inverse() * _projMat.inverse();
    auto m = transform().matrix();
    p4     = m.inverse() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::worldToEye(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    p4     = transform().inverse().matrix() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::worldToEye(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    p4     = transform().inverse().matrix() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::screenToWorld(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    // o = p4 * _viewMat.inverse() * _projMat.inverse() *
    // _viewportMat.inverse();
    auto m = _projMat * transform().matrix();
    p4[0] = (p[0] - _viewport[0] - _viewport[2] / 2.0f) * 2.0f / _viewport[2];
    p4[1] = (p[1] - _viewport[1] - _viewport[3] / 2.0f) * 2.0f / _viewport[3];
    p4[2] = (p[2] - 0.5f) * 2.0f;
    p4[3] = 1.0f;
    p4    = m.inverse() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::screenToWorld(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    // o = p4 * _viewMat.inverse() * _projMat.inverse() *
    // _viewportMat.inverse();
    auto m = _projMat * transform().matrix();
    o      = m.inverse() * p4;

    return Vector3(o.data());
}

Vector3 Camera::worldToScreen(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    o      = _viewportMat * _projMat * transform().matrix() * p4;
    Vector3 t(o.data());
    return t;
}

Vector3 Camera::worldToScreen(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    o      = _viewportMat * _projMat * transform().matrix() * p4;
    Vector3 t(o.data());
    return t;
}

Vector3 Camera::eyeToClip(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    // o = p4 * _viewMat.inverse() * _projMat.inverse();
    auto m = _projMat * transform().matrix();
    p4     = m.inverse() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::eyeToClip(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    // o = p4 * _viewMat.inverse() * _projMat.inverse();
    auto m = _projMat * transform().matrix();
    p4     = m.inverse() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::clipToEye(const Vector3& p) const
{
    Eigen::Vector4f o, p4(p.x(), p.y(), p.z(), 1.0f);
    p4.w() = 1.0f;
    p4     = _projMat * transform().matrix() * p4;
    return Vector3(p4.data());
}

Vector3 Camera::clipToEye(float x, float y, float z, float w) const
{
    Eigen::Vector4f o, p4(x, y, z, w);
    p4.w() = w;
    p4     = _projMat * transform().matrix() * p4;
    return Vector3(p4.data());
}
