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

#include <iostream>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <memory>
#include <functional>

#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <iterator>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <OpenGL/OpenGL.h>
//#include <OpenGL/OpenGL.h>
//#include <OpenGL/glu.h>
#else
#include <gl/gl3.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#endif /* __APPLE__ */


namespace Eigen {
    typedef Affine3f Transform3f;    
};

#include "PEngineTypes.h"

#include "GlException.h"


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
#endif /* PLASTILINA_GL_EXCEPTON_ENABLE */

#define RET_ON_GLERROR(val) { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            return val; \
        }\
    }



template <typename T>
inline void hash_combine(std::size_t & seed, const T & v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
    template<typename S, typename T> struct hash<pair<S, T>>
    {
        inline size_t operator()(const pair<S, T> & v) const
        {
            size_t seed = 0;
            ::hash_combine(seed, v.first);
            ::hash_combine(seed, v.second);
            return seed;
        }
    };
}

#endif /* qsculpt_Plastilina_h */
