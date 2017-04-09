//
//  GlException.h
//  qsculpt
//
//  Created by Juan Cabral on 1/29/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_GlException_h
#define qsculpt_GlException_h

#include <exception>
#include <stdexcept>
#include "OpenGL.h"

namespace core
{
class GlException : public std::exception
{
    const char* what_;
    GLenum      error_;

  public:
    GlException(const char* what, GLenum error) throw()
        : std::exception()
        , what_(what)
        , error_(error)
    {
    }

    virtual ~GlException() throw()
    {
        what_  = NULL;
        error_ = 0;
    }

    virtual const char* what() const throw() { return what_; }

    GLenum error() const throw() { return error_; }

    const char* errorString() const throw()
    {
	#define GLENUMTOSTR(x) case x: return #x
	switch(error_)
	{
	GLENUMTOSTR(GL_NO_ERROR);
	GLENUMTOSTR(GL_INVALID_ENUM);
	GLENUMTOSTR(GL_INVALID_OPERATION);
	GLENUMTOSTR(GL_INVALID_VALUE);
	GLENUMTOSTR(GL_INVALID_FRAMEBUFFER_OPERATION);
	GLENUMTOSTR(GL_OUT_OF_MEMORY);
	}
        return "UNKONWN";
    }
};
}

#endif
