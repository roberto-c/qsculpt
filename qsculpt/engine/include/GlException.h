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

namespace core {
    class GlException : public std::exception 
    {
        const char* what_;
        GLenum error_;
        
    public:
        GlException(const char* what, GLenum error) : what_(what), error_(error)
        {}
        
        virtual const char* what() const throw() {
            return what_;
        }
        
        GLenum error() const {
            return error_;
        }
        
        const char* errorString() const {
            return (const char*)gluErrorString(error_);
        }
    };
}

#endif
