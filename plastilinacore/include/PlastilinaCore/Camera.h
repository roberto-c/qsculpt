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

#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/SceneNode.h>


/**
 * @class Camera
 *
 * The Camera class provides methods to create the different matrices used
 * to transform the vertices in world space to screen space.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Camera : public CameraNode
{
public:
    typedef std::shared_ptr<Camera>       	shared_ptr;
    typedef std::weak_ptr<Camera>    		weak_ptr;
    typedef std::shared_ptr<const Camera> 	const_shared_ptr;
    typedef std::weak_ptr<const Camera>		const_weak_ptr;
    
    Camera();

    Camera(const Camera & orig);
    
    virtual ~Camera();
    
    Camera & operator=(const Camera & orig);

    void setPosition(const Point3& p);

    Point3 getPosition();
    
    void setTargetPoint(const Point3 & target);
    
    Point3 getTargetPoint();

    void setOrientationVector(const Point3& v);

    Point3 getOrientationVector();

    void setLongitude(double longitude);

    double getLongitude();

    void setColatitude(double colatitude);

    double getColatitude();

    void setDistanceFromTarget(double distance);

    double getDistanceFromTarget();

    /**
     * Set the transformation matrix to use for the camera or eye
     * transformation.
     *
     * If this function is used, then position, target and orientation vectors
     * are not valid anymore as those paramater are used to construct this
     * matrix.
     *
     */
    void setModelView(const Eigen::Matrix4f& m);

    /**
     * Returns the model view matrix used by this camera.
     */
    const Eigen::Matrix4f& modelView();
    
    /**
     * Set the projection matrix to use for the perspective projection
     * transformation.
     *
     * @param m projection matrix to use
     *
     */
    void setProjectionMatrix(const Eigen::Matrix4f& m);

    /**
     * Returns the projection matrix used by the camera
     */
    const Eigen::Matrix4f& projection();

    /**
     * Constructs a new perspective projection matrix based in the specified
     * planes values.
     */
    void setPerspectiveMatrix(float left, float right,
                              float bottom, float top,
                              float near, float far);

    /**
     * Constructs a new orthographic projection matrix based in the specified
     * planes values.
     */
    void setOrthoMatrix(float left, float right,
                        float bottom, float top,
                        float near, float far);

    /**
     * Construct a perspective projection matrix based in field of view and 
     * aspect ratio values.
     *
     * @param fovy the field of view angle in degrees in the y direction
     * @param aspect aspect ratio used to determine the field of view in the x direction
     * @param zNear cut off plane in the z direction
     * @param zFar cut off plane in the z direction, far from the point of view.
     *
     */
    void setPerspectiveMatrix(float fovy, float aspect, float zNear, float zFar);
    
    /**
     * Set the viewport transformation matrix.
     *
     * This transformation matrix is applied after the projection transformation
     * to map from the projection canonical view volume to screen space or 
     * window space.
     *
     * This is similar to calling glViewpot in OpenGL.
     */
    void setViewport(const Eigen::Matrix4f& m);

    /**
     * Set the viewport matrix.
     *
     * @param x the initial x window coordinate
     * @param y the initial y window coordinate
     * @param w the width of the viewport
     * @param h the height of the viewport
     */
    void setViewport(int x, int y, int w, int h);

    /**
     * Returns the viewport transformation matrix.
     *
     * @see Camera::setViewport
     */
    const Eigen::Matrix4f& viewport() const;
    
    /**
     * Maps a point from screen space to world space.
     *
     * This method is similar in purpose to gluUnproject function call.
     */
    Vector3 eyeToWorld(const Vector3& p) const;
    
    /**
     * Overload to accept a vector by components
     */
    Vector3 eyeToWorld(float x, float y, float z, float w = 1.f) const;
    
    /**
     * Maps a point from world space to screen space.
     *
     * This method is similar in purpose to gluProject function call.
     */
    Vector3 worldToEye(const Vector3& p) const;

    /**
     * Overload to accept a vector by components
     */
    Vector3 worldToEye(float x, float y, float z, float w = 1.f) const;

    
    /**
     * Create a string representation of the object. 
     *
     * Outputs each attribute this class contains.
     */
    std::string toString() const;

private:
    void updateViewMatrix();
    
    Eigen::Matrix4f     _projMat;
    Eigen::Matrix4f     _viewMat;
    Eigen::Matrix4f     _viewportMat;
    
    Point3 m_position;
    Point3 m_target;
    Point3 m_orientation;

    double m_colatitude;
    double m_longitude;
    double m_distanceFromTarget;
};

#endif
