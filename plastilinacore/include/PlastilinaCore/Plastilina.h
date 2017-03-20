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
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <PlastilinaCore/Config.h>
#include <PlastilinaCore/Logging.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>
#include <Eigen/StdVector>
#include <iterator>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif /* __APPLE__ */

#ifdef __OBJC__
#define OBJC_CLASS(name) @class name
#else
#define OBJC_CLASS(name) typedef struct objc_object name
#endif

#ifdef _MSC_VER
#define THREAD __declspec(thread)
#define DLLEXPORT
//#ifdef PLASTILINACORE_EXPORT
//#define DLLEXPORT __declspec( dllexport )
//#else
//#define DLLEXPORT __declspec( dllimport )
//#endif
#else
#define THREAD __thread
#define DLLEXPORT
#endif

namespace Eigen
{
typedef Affine3f Transform3f;
};

#include "PEngineTypes.h"

#include "Variant.h"
#include "opengl/GlException.h"

#define NOT_IMPLEMENTED throw std::runtime_error("Not implemented");

#ifdef PLASTILINA_GL_EXCEPTON_ENABLE
#define THROW_IF_GLERROR(msg)                                                \
    {                                                                        \
        GLenum error = glGetError();                                         \
        if (error != GL_NO_ERROR)                                            \
        {                                                                    \
            throw core::GlException(msg, error);                             \
        }                                                                    \
    }
#else
#define THROW_IF_GLERROR(msg)
#endif /* PLASTILINA_GL_EXCEPTON_ENABLE */

#define RET_ON_GLERROR(val)                                                  \
    {                                                                        \
        GLenum error = glGetError();                                         \
        if (error != GL_NO_ERROR)                                            \
        {                                                                    \
            return val;                                                      \
        }                                                                    \
    }

#define LOG_IF_GLERROR(msg)                                                  \
    {                                                                        \
        GLenum error = glGetError();                                         \
        if (error != GL_NO_ERROR)                                            \
        {                                                                    \
            TRACE(error) << "glGetError: " << error << ": " << msg;          \
        }                                                                    \
    }

template <typename T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
template <typename S, typename T>
struct hash<pair<S, T>>
{
    inline size_t operator()(const pair<S, T>& v) const
    {
        size_t seed = 0;
        ::hash_combine(seed, v.first);
        ::hash_combine(seed, v.second);
        return seed;
    }
};
};

#define DECLARE_ENUM_FLAGS_BEGIN(name)                                       \
    enum class name : unsigned int                                           \
    {
#define DECLARE_ENUM_FLAGS_END(name)                                         \
    }                                                                        \
    ;                                                                        \
    \
inline name                                                                  \
    operator~(name lhs)                                                      \
    {                                                                        \
        using T = std::underlying_type<name>::type;                          \
        return name(~static_cast<T>(lhs));                                   \
    \
}                                                                     \
    \
inline name                                                                  \
    operator|(name lhs, name rhs)                                            \
    {                                                                        \
        using T = std::underlying_type<name>::type;                          \
        return name(static_cast<T>(lhs) | static_cast<T>(rhs));              \
    \
}                                                                     \
    \
inline name                                                                  \
    operator&(name lhs, name rhs)                                            \
    {                                                                        \
        using T = std::underlying_type<name>::type;                          \
        return name(static_cast<T>(lhs) & static_cast<T>(rhs));              \
    \
}                                                                     \
    \
inline name                                                                  \
    operator^(name lhs, name rhs)                                            \
    {                                                                        \
        using T = std::underlying_type<name>::type;                          \
        return name(static_cast<T>(lhs) ^ static_cast<T>(rhs));              \
    \
}

// enum class PlastilinaSubsystem {
DECLARE_ENUM_FLAGS_BEGIN(PlastilinaSubsystem)
NONE = 0, OPENGL = 0x0001, OPENCL = 0x0002, ENABLE_CL_GL_SHARING = 0x0004,
          VULKAN = 0x0008 DECLARE_ENUM_FLAGS_END(PlastilinaSubsystem)
          //};

          DECLARE_ENUM_FLAGS_BEGIN(PlastilinaFeatureBitFlag) NONE = 0,
          OPENGL = 0x0001, OPENCL = 0x0002, ENABLE_CL_GL_SHARING = 0x0004,
          VULKAN = 0x0008 DECLARE_ENUM_FLAGS_END(PlastilinaFeatureBitFlag)

              namespace core
{
    class Context;

    DECLARE_ENUM_FLAGS_BEGIN(ApiSupported)
    NONE = 0, OPENGL = 0x0001, OPENCL = 0x0002,
    VULKAN = 0x0004 DECLARE_ENUM_FLAGS_END(ApiSupported)
};

typedef std::map<std::string, core::Variant> AttributeMap;

class DLLEXPORT PlastilinaEngine
{
  public:
    static bool initialize(PlastilinaSubsystem subsystem);

    static bool initializeWithAttributes(AttributeMap attr);

    static bool initializeFromCommandLine(int argc, const char** athv);

    static bool initializeFromConfigFile(const std::string& filepath);

    static bool shutdown();

    static void setCurrentContext(std::shared_ptr<core::Context>& ctx);

    static core::Context& currentContext();

    static bool supportsFeature(PlastilinaFeatureBitFlag flags);
};

#define ENGINE_NAME u8"PlastilinaCore"

#endif /* qsculpt_Plastilina_h */
