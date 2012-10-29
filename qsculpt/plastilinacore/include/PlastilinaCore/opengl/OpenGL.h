//
//  OpenGL.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 7/28/12.
//
//

#ifndef PlastilinaCore_OpenGL_h
#define PlastilinaCore_OpenGL_h

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

/* to avoid including gl.h*/
#define __gl_h_

#else
#include <GL/gl3.h>
#include <GL/gl3ext.h>
#endif

#endif
