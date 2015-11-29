//
//  VertexArrayObject.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/19/12.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

namespace core {
namespace opengl {

    VertexArrayObject::VertexArrayObject()
    {
        glGenVertexArrays(1, &vao_);
        THROW_IF_GLERROR("VertexArrayObject");
    }
    
    VertexArrayObject::~VertexArrayObject()
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    
    GLuint VertexArrayObject::objectID() const
    {
        return vao_;
    }
    
    void VertexArrayObject::bind() const
    {
        glBindVertexArray(vao_);
        THROW_IF_GLERROR("VertexArrayObject::bind");
    }
    
    void VertexArrayObject::release() const
    {
        glBindVertexArray(0);
    }
}
}