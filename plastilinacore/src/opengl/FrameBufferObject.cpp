/***************************************************************************
*   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
*   roberto.cabral@gmail.com                                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include <PlastilinaCore/Stable.h>

#include <PlastilinaCore/opengl/FrameBufferObject.h>
#include <PlastilinaCore/opengl/Texture.h>

namespace gl
{
FrameBufferObject::FrameBufferObject()
    : gl::Object<FrameBufferObject>()
{
}
FrameBufferObject::~FrameBufferObject()
{
}

void FrameBufferObject::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, oglname_);
    THROW_IF_GLERROR("Failed to bind framebuffer object");
}
void FrameBufferObject::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    THROW_IF_GLERROR("Failed to bind framebuffer object");
}
void FrameBufferObject::framebufferTexture(GLenum attachment, Texture2D & texture, GLint level)
{
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.oid(), level);
    THROW_IF_GLERROR("Failed to attach texture image to framebuffer object");
}
void FrameBufferObject::framebufferRenderbuffer(GLenum attachment, GLenum renderbuffertarget, RenderBuffer & renderbuffer)
{
    glFramebufferRenderbuffer(GL_RENDERBUFFER, attachment, renderbuffertarget, renderbuffer.oid());
    THROW_IF_GLERROR("FrameBufferObject::framebufferRenderbuffer: failed to attach RenderBuffer");
}
bool FrameBufferObject::isComplete() const
{
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return status == GL_FRAMEBUFFER_COMPLETE;
}
void FrameBufferObject::create(uint32_t n, GLuint * name)
{
    glGenFramebuffers(n, name);
    THROW_IF_GLERROR("Failed to create FrameBufferObject");
}

void FrameBufferObject::destroy(uint32_t n, GLuint * name)
{
    glDeleteFramebuffers(n, name);
    THROW_IF_GLERROR("Failed to destroy framebuffer object");
}
RenderBuffer::RenderBuffer()
    : gl::Object<RenderBuffer>()
{
}
RenderBuffer::~RenderBuffer()
{
}
void RenderBuffer::bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, oglname_);
    THROW_IF_GLERROR("RenderBuffer::bind: Failed to bind RenderBuffer");
}
void RenderBuffer::unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    THROW_IF_GLERROR("RenderBuffer::bind: Failed to bind RenderBuffer");
}
void RenderBuffer::storage(GLenum internalformat, GLsizei width, GLsizei height)
{
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    THROW_IF_GLERROR("RenderBuffer::storage: failed to set storage");
}
void RenderBuffer::storageMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, width, height);
    THROW_IF_GLERROR("RenderBuffer::storageMultisample: failed to set storage");
}
void RenderBuffer::create(uint32_t n, GLuint * name)
{
    glGenRenderbuffers(n, name);
    THROW_IF_GLERROR("RenderBuffer::create: Failed to create RenderBuffer");
}
void RenderBuffer::destroy(uint32_t n, GLuint * name)
{
    glDeleteRenderbuffers(n, name);
    THROW_IF_GLERROR("RenderBuffer::destroy: Failed to delete RenderBuffer");
}
};
