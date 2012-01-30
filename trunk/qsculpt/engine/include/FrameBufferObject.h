//
//  FrameBufferObject.h
//  qsculpt
//
//  Created by Juan Cabral on 12/27/11.
//  Copyright (c) 2011 plastilinaware. All rights reserved.
//

#ifndef qsculpt_FrameBufferObject_h
#define qsculpt_FrameBufferObject_h

#include "Texture.h"

class FrameBufferObject : private Texture
{
public:
    FrameBufferObject();
    ~FrameBufferObject();
    
    /**
     * get the target to which this framebuffer is going to be bound.
     */
    GLenum target() const;
    
    /**
     * Return the name id of the OpenGL object.
     */
    GLuint oid() const;
    
    /**
     * Bind the framebuffer object using the target
     */
    GLenum bind();
    
    /**
     * Unbind the framebuffer
     */
    GLenum unbind();
};

#endif
