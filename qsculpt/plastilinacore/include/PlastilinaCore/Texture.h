//
//  Texture.h
//  qsculpt
//
//  Created by Juan Cabral on 12/27/11.
//  Copyright (c) 2011 plastilinaware. All rights reserved.
//

#ifndef qsculpt_Texture_h
#define qsculpt_Texture_h

class Texture
{
public:
    Texture();
    Texture(GLuint width, GLuint height, GLenum flags);
    ~Texture();
    
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
