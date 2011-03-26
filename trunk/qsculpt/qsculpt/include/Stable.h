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
#ifndef STABLE_H_
#define STABLE_H_ 

#if defined __cplusplus

#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/StdVector>

namespace Eigen {
    typedef Affine3f Transform3f; 
}

#include <utility>
#include <typeinfo>

unsigned int qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key);
unsigned int qHash(const std::pair<int, int> & v);

#include <QtDebug>
#include <QtGui>
#include <QtOpenGL>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QExplicitlySharedDataPointer>

inline bool printGlError()
{
    GLuint error = glGetError();
	bool result = (error == GL_NO_ERROR);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}

#define NOT_IMPLEMENTED qWarning("%s %s", __PRETTY_FUNCTION__, "not implemented");

#endif /* defined __cplusplus */

#endif /* STABLE_H_ */
