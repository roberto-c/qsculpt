/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
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
#pragma once

#include <GL/glew.h>
#include "GlException.h"

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
