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


#ifdef __OBJC__
#define OBJC_CLASS(name) @class name
#else
#define OBJC_CLASS(name) typedef struct objc_object name
#endif

#ifdef _MSC_VER
#define WARNINGS_DISABLE __pragma(warning(push, 0))
#define WARNINGS_ENABLE __pragma(warning(pop))
#define THREAD __declspec(thread)
#define DLLEXPORT
//#ifdef PLASTILINACORE_EXPORT
//#define DLLEXPORT __declspec( dllexport )
//#else
//#define DLLEXPORT __declspec( dllimport )
//#endif
#else
#define WARNINGS_DISABLE _Pragma("GCC diagnostic push")
#define WARNINGS_ENABLE _Pragma("GCC diagnostic pop")
#define THREAD __thread
#define DLLEXPORT
#endif

#define NOT_IMPLEMENTED throw std::runtime_error("Not implemented");
