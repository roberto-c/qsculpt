//
//  VertexArrayObject.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/19/12.
//
//

#ifndef __PlastilinaCore__VertexArrayObject__
#define __PlastilinaCore__VertexArrayObject__

#include <iostream>

#include <PlastilinaCore/opengl/OpenGL.h>

namespace core {
namespace opengl {
    class VertexArrayObject {
        GLuint vao_;
        
    public:
        /**
         * Contructs a Vertex Array Object.
         *
         * The OpenGL name or ID is generated at time of constructions.
         */
        VertexArrayObject();
        
        /**
         * Frees all resources from this VAO
         */
        ~VertexArrayObject();
        
        /**
         * Returns the OpenGL name for this VAO.
         *
         * @return ID of the vao for OpenGL queries.
         */
        GLuint objectID() const;
        
        /**
         * Bind the VAO
         */
        void bind() const;
        
        /**
         * Unbind or release the vertex array object from the current OpenGL 
         * state.
         */
        void release() const;
    };
}
}

typedef core::opengl::VertexArrayObject VAO;

#endif /* defined(__PlastilinaCore__VertexArrayObject__) */
