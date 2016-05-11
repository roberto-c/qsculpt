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

namespace gl {
    class DLLEXPORT VertexArrayObject {
        GLuint vao_;
        
    public:
        /**
         * Contructs a Vertex Array Object.
         *
         * The OpenGL name or ID is generated at time of constructions.
         */
        VertexArrayObject();
        VertexArrayObject(const VertexArrayObject& cpy) = delete;
        VertexArrayObject(VertexArrayObject&& cpy) = delete;
        
        /**
         * Frees all resources from this VAO
         */
        ~VertexArrayObject();
        
        VertexArrayObject& operator=(const VertexArrayObject& other) = delete;
        VertexArrayObject& operator=(VertexArrayObject&& other) = delete;

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
        void unbind() const;
    };
}

typedef gl::VertexArrayObject VAO;

#endif /* defined(__PlastilinaCore__VertexArrayObject__) */
