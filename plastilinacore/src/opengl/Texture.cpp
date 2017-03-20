/***************************************************************************
 *   Copyright (C) 2013 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/opengl/Texture.h>

namespace gl
{

void TextureManager::setActiveTexture(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    THROW_IF_GLERROR("Failed to activate texture");
    currentTextureUnit_ = textureUnit;
}

template <int TextureTarget>
struct Texture<TextureTarget>::Impl
{
    GLuint               name;
    GLsizei              width, height, depth;
    GLenum               format;
    GLenum               target;
    GLint                level;
    GLint                internalFormat;
    GLint                border;
    GLenum               type;
    std::vector<GLubyte> data;

    Impl()
        : name(0)
        , width(0)
        , height(0)
        , depth(1)
        , format(0)
        , target(TextureTarget)
        , level(0)
        , internalFormat(GL_RGB)
        , border(0)
        , type(0)
    {
    }

    Impl(GLsizei width, GLsizei height = 1, GLsizei depth = 1,
         GLenum flags = 0)
        : name(0)
        , width(width)
        , height(height)
        , depth(depth)
        , format(0)
        , target(TextureTarget)
        , level(0)
        , internalFormat(GL_RGB)
        , border(0)
        , type(0)
    {
    }
};

template <int TextureTarget>
Texture<TextureTarget>::Texture()
    : d(new Impl)
{
    glGenTextures(1, &d->name);
}

template <int TextureTarget>
Texture<TextureTarget>::Texture(GLuint width, GLuint height, GLuint depth,
                                GLenum flags)
    : d(new Impl(width, height, depth, flags))
{
    glGenTextures(1, &d->name);
}

template <int TextureTarget>
Texture<TextureTarget>::~Texture()
{
    glDeleteTextures(1, &d->name);
}

/**
 * get the target to which this framebuffer is going to be bound.
 */
template <int TextureTarget>
GLenum        Texture<TextureTarget>::target() const
{
    return TextureTarget;
}

/**
 * Return the name id of the OpenGL object.
 */
template <int TextureTarget>
GLuint        Texture<TextureTarget>::oid() const
{
    return d->name;
}

/**
 * Bind the framebuffer object using the target
 */
template <int TextureTarget>
void          Texture<TextureTarget>::bind()
{
    glBindTexture(TextureTarget, d->name);
    THROW_IF_GLERROR("Failed to bind texture");
}

/**
 * Unbind the framebuffer
 */
template <int TextureTarget>
void          Texture<TextureTarget>::unbind()
{
    glBindTexture(TextureTarget, 0);
    THROW_IF_GLERROR("Failed to unbind texture");
}

template <int TextureTarget>
void Texture<TextureTarget>::setParameter(GLenum pname, GLint value)
{
    glTexParameteri(TextureTarget, pname, value);
    THROW_IF_GLERROR("Failed to set paramter");
}

template <int TextureTarget>
void Texture<TextureTarget>::setParameter(GLenum pname, GLfloat value)
{
    glTexParameterf(TextureTarget, pname, value);
    THROW_IF_GLERROR("Failed to set paramter");
}

template <int TextureTarget>
void Texture<TextureTarget>::setInternalFormat(GLint format, GLenum pname,
                                               GLint value)
{
    NOT_IMPLEMENTED
}

template <int TextureTarget>
GLint Texture<TextureTarget>::internalFormat(GLint format, GLenum pname) const
{
    NOT_IMPLEMENTED;
    return GL_NONE;
}

// instantiate Texture<GL_TEXTURE_2D>
template class Texture<GL_TEXTURE_2D>;

//
// Texture2D definition

Texture2D::Texture2D()
    : Texture<GL_TEXTURE_2D>(1, 1, 1, 0)
{
    bind();
    setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    setParameter(GL_TEXTURE_MAX_LEVEL, 0);
}

Texture2D::Texture2D(GLuint width, GLuint height, GLenum flags)
    : Texture<GL_TEXTURE_2D>(width, height, 1, flags)
{
    bind();
    setParameter(GL_TEXTURE_BASE_LEVEL, 0);
    setParameter(GL_TEXTURE_MAX_LEVEL, 0);
}

Texture2D::~Texture2D() {}

void Texture2D::texImage2D(GLint level, GLint internalformat, GLsizei width,
                           GLsizei height, GLint border, GLenum format,
                           GLenum type, const void* data)
{
    glTexImage2D(Texture2D::Target, level, internalformat, width, height,
                 border, format, type, data);
    THROW_IF_GLERROR("Failed to upload texture data");
    d->width  = width;
    d->height = height;
    d->depth  = 1;
    d->border = border;
    d->format = format;
    d->level  = level;
}

void Texture2D::copyImageSubDataTo(GLint srcLevel, GLint srcX, GLint srcY,
                                   Texture2D dst, GLint dstLevel, GLint dstX,
                                   GLint dstY, GLsizei srcWidth,
                                   GLsizei srcHeight)
{
    if (glewIsExtensionSupported("GL_ARB_copy_image"))
    {
        glCopyImageSubData(this->oid(), Texture2D::Target, srcLevel, srcX,
                           srcY, 0, dst.oid(), dst.target(), dstLevel, dstX,
                           dstY, 0, srcWidth, srcHeight, 0);
        THROW_IF_GLERROR("Failed to upload texture data");
    }
    else
    {
        NOT_IMPLEMENTED;
    }
}

} /* End gl namespace */
