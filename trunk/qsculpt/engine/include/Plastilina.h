/***************************************************************************
 *   Copyright (C) 2012 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
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

#ifndef qsculpt_Plastilina_h
#define qsculpt_Plastilina_h

#include <exception>
#include <stdexcept>
#include <typeinfo>

#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <iterator>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#include <gl/glu.h>
#endif


namespace Eigen {
    typedef Affine3f Transform3f;    
};


unsigned int qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key);
unsigned int qHash(const std::pair<int, int> & v);

#include <QtCore/QtDebug>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QString>
#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QSharedData>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QSharedDataPointer>
#include "PEngineTypes.h"

#include "GlException.h"

inline bool printGlError()
{
	bool result = false;
	GLenum error = glGetError();
	while( error != GL_NO_ERROR )
	{
		result = true;
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
		error = glGetError();
	}
	return result;
}

#define NOT_IMPLEMENTED throw std::runtime_error("Not implemented");

#ifdef PLASTILINA_GL_EXCEPTON_ENABLE
#define THROW_IF_GLERROR(msg) { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            throw core::GlException(msg, error); \
        }\
    }
#else
#define THROW_IF_GLERROR(msg)
#endif

#define RET_ON_GLERROR(val) { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            return val; \
        }\
    }

#endif
