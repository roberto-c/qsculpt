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
#ifndef POINT3D_H
#define POINT3D_H

#include <Eigen/Geometry>
#include <vector>

typedef Eigen::Vector2f Point2;
typedef Eigen::Vector2f Vector2;
typedef Eigen::Vector3f Point3;
typedef Eigen::Vector3f Vector3;
typedef Eigen::Vector4f Point4;
typedef Eigen::Vector4f Vector4;

typedef std::vector<int> PointIndexList;

// Q_DECLARE_METATYPE(Point3)

inline std::string toString(const Vector3& v)
{
    std::ostringstream str;
    //    str << v;
    str << "(" << v[0] << "," << v[1] << "," << v[2] << ")";

    return str.str();
}

inline std::string toString(const Vector4& v)
{
    std::ostringstream str;
    str << "(" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ")";
    // str << v;
    //	std::string str;
    //	str += "(" + std::string::number(v[0]) + "," +
    //	std::string::number(v[1]) + "," +
    //	std::string::number(v[2]) + "," +
    //    std::string::number(v[3]) + ")";
    return str.str();
}

#endif
