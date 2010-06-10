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

#include <QMetaType>
#include <QString>
#include <QVector>

typedef Eigen::Vector3f Point3;
typedef Eigen::Vector3f Vector3;

typedef QVector<int> PointIndexList;

Q_DECLARE_METATYPE(Point3)

inline QString toString(const Vector3& v)
{
	QString str;
	str += "(" + QString::number(v[0]) + "," + 
	QString::number(v[1]) + "," +
	QString::number(v[2]) + ")";
	return str;
}

#endif

