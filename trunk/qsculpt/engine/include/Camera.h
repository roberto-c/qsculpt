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

#include "ICamera.h"

/**
 * @class Camera
 *
 * The Camera class provides methods to create the different matrices used
 * to transform the vertices in world space to screen space.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Camera
{
public:
    Camera();

    virtual ~Camera();

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
     * Set the transformation matrix to use for the camera or eye transformation.
     *
     * If this function is used, then position, target and orientation vectors
     * are not valid anymore as those paramater are used to construct this matrix.
     *
     */
    void setModelView(const Eigen::Matrix4f& m);
    
    const Eigen::Matrix4f& modelView();
    
    void setProjectionMatrix(const Eigen::Matrix4f& m);
    
    const Eigen::Matrix4f& projection();
    
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
    Vector3 screenToWorld(const Vector3& p) const;
    
    /**
     * Maps a point from world space to screen space.
     *
     * This method is similar in purpose to gluProject function call.
     */
    Vector3 worldToScreen(const Vector3& p) const;
    
    
    /**
     * Create a string representation of the object. 
     *
     * Outputs each attribute this class contains.
     */
    QString toString() const;

private:
    void updateMatrix();
    
    Eigen::Matrix4f     _projMat;
    Eigen::Matrix4f     _viewPort;
    
    Point3 m_position;
    Point3 m_target;
    Point3 m_orientation;

    double m_colatitude;
    double m_longitude;
    double m_distanceFromTarget;
};

#endif
