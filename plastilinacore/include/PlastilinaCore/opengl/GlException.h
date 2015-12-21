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

namespace core {
    class GlException : public std::exception
    {
        const char* what_;
        GLenum error_;
        
    public:
        GlException(const char* what, GLenum error) throw()
            : std::exception(), what_(what), error_(error)
        {}
        
        virtual ~GlException() throw()
        {
            what_ = NULL;
            error_ = 0;
        }
        
        virtual const char* what() const throw() {
            return what_;
        }
        
        GLenum error() const throw() {
            return error_;
        }
        
        const char* errorString() const throw() {
            return (const char*)gluErrorString(error_);
        }
    };
}

#endif
