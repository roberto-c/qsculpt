/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
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

#ifndef qsculpt_Texture_h
#define qsculpt_Texture_h

#include <PlastilinaCore/opengl/OpenGL.h>

namespace gl
{

class DLLEXPORT TextureManager
{
    GLenum currentTextureUnit_;

  public:
    static TextureManager* instance()
    {
        static TextureManager* inst = NULL;
        if (!inst)
        {
            inst = new TextureManager;
        }
        return inst;
    }

    GLenum activeTexture() const { return currentTextureUnit_; }
    void setActiveTexture(GLenum textureUnit);

  private:
    TextureManager()
        : currentTextureUnit_(GL_TEXTURE0)
    {
    }
};

template <int   TextureTarget>
class DLLEXPORT Texture
{
  public:
    typedef std::shared_ptr<Texture<TextureTarget>>       shared_ptr;
    typedef std::weak_ptr<Texture<TextureTarget>>         weak_ptr;
    typedef std::shared_ptr<const Texture<TextureTarget>> const_shared_ptr;
    typedef std::weak_ptr<const Texture<TextureTarget>>   const_weak_ptr;

    virtual ~Texture();

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
    void bind();

    /**
     * Unbind the framebuffer
     */
    void unbind();

    void setInternalFormat(GLint format, GLenum pname, GLint value);

    GLint internalFormat(GLint format, GLenum pname) const;

    void setParameter(GLenum pname, GLint value);

    void setParameter(GLenum pname, GLfloat value);

  protected:
    enum
    {
        Target = TextureTarget
    };

    Texture();

    Texture(GLuint width, GLuint height, GLuint depth, GLenum flags);

    struct Impl;
    std::unique_ptr<Impl> d;
};

class DLLEXPORT Texture2D : public Texture<GL_TEXTURE_2D>
{
  public:
    typedef std::shared_ptr<Texture2D>       shared_ptr;
    typedef std::weak_ptr<Texture2D>         weak_ptr;
    typedef std::shared_ptr<const Texture2D> const_shared_ptr;
    typedef std::weak_ptr<const Texture2D>   const_weak_ptr;

    Texture2D();
    Texture2D(GLuint width, GLuint height, GLenum flags);
    virtual ~Texture2D();

    void texImage2D(GLint level, GLint internalformat, GLsizei width,
                    GLsizei height, GLint border, GLenum format, GLenum type,
                    const void* data);

    void copyImageSubDataTo(GLint srcLevel, GLint srcX, GLint srcY,
                            Texture2D dst, GLint dstLevel, GLint dstX,
                            GLint dstY, GLsizei srcWidth, GLsizei srcHeight);
};
}; /* End namespace gl*/

#endif
