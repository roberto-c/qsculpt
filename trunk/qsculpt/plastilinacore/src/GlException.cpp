//
//  GlException.cpp
//  qsculpt
//
//  Created by Juan Cabral on 2/14/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#include <iostream>
#include <PlastilinaCore/GlException.h>

namespace core {
    
    GlException::GlException(const char* what, GLenum error) throw() 
        : std::exception(), what_(what), error_(error)
            {}
            
    GlException::~GlException() throw() {
        what_ = NULL;
        error_ = 0;
    }
            
    const char* GlException::what() const throw() {
        return what_;
    }

    GLenum GlException::error() const throw() {
        return error_;
    }

    const char* GlException::errorString() const throw() {
        return (const char*)gluErrorString(error_);
    }
}
