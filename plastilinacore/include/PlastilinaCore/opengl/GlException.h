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
    class DLLEXPORT GlException : public std::exception
    {
        const char* what_;
        GLenum error_;
        
    public:
        GlException(const char* what, GLenum error) throw();
        
        virtual ~GlException() throw();
        
        virtual const char* what() const throw() ;
        
        GLenum error() const throw() ;
        
        const char* errorString() const throw() ;
    };
}

#endif
