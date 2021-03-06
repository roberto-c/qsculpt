/***************************************************************************
 *   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
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
#ifndef PlastilinaCore_opengl_Object_h
#define PlastilinaCore_opengl_Object_h

#include <PlastilinaCore/opengl/OpenGL.h>

namespace gl
{
template <typename T>
class Object
{
  protected:
    GLuint oglname_;

  public:
    Object()
        : oglname_(0)
    {
        T::create(1, &oglname_);
    }

    virtual ~Object() { T::destroy(1, &oglname_); }

    GLuint oid() const { return oglname_; }
};
};

#endif
