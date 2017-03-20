//
//  FrameBufferObject.h
//  qsculpt
//
//  Created by Juan Cabral on 12/27/11.
//  Copyright (c) 2011 plastilinaware. All rights reserved.
//

#ifndef qsculpt_FrameBufferObject_h
#define qsculpt_FrameBufferObject_h

#include <PlastilinaCore/opengl/Object.h>

namespace gl
{
class Texture2D;
class RenderBuffer;

class FrameBufferObject : public gl::Object<gl::FrameBufferObject>
{
  public:
    FrameBufferObject();

    virtual ~FrameBufferObject();

    /**
     * Bind the framebuffer object using the target
     */
    void bind();

    /**
     * Unbind the framebuffer
     */
    void unbind();

    void framebufferTexture(GLenum attachment, Texture2D& texture,
                            GLint level);

    void framebufferRenderbuffer(GLenum attachment, GLenum renderbuffertarget,
                                 RenderBuffer& renderbuffer);

    bool isComplete() const;

    static void create(uint32_t n, GLuint* name);
    static void destroy(uint32_t n, GLuint* name);
};

class RenderBuffer : public gl::Object<gl::RenderBuffer>
{
  public:
    RenderBuffer();

    virtual ~RenderBuffer();

    /**
     * Bind the framebuffer object using the target
     */
    void bind();

    /**
     * Unbind the framebuffer
     */
    void unbind();

    void storage(GLenum internalformat, GLsizei width, GLsizei height);

    void storageMultisample(GLsizei samples, GLenum internalformat,
                            GLsizei width, GLsizei height);

    static void create(uint32_t n, GLuint* name);
    static void destroy(uint32_t n, GLuint* name);
};
};
#endif
