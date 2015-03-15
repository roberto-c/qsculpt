//
//  Texture.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 10/18/13.
//
//

#include <PlastilinaCore/opengl/Texture.h>

namespace gl {

template <int TextureTarget>
template <int TextureUnit2>
struct Texture<TextureTarget>::Impl {
	GLuint  name;
	GLsizei width, height, depth;
	GLenum  format;
	GLenum  target;
	GLint   level;
	GLint   internalFormat;
	GLint   border;
	GLenum  type;
	std::vector<GLubyte> data;
	
	Impl()
	:name(0),width(0),height(0),depth(1),format(0),target(TextureUnit2) {
	}
	
	Impl(GLsizei width, GLsizei height = 1, GLsizei depth = 1, GLenum flags = 0)
	:name(0),width(width),height(height),depth(depth),format(0),
	 target(TextureUnit2) {
	}
	
};

template <int TextureTarget>
Texture<TextureTarget>::Texture() : d(new Impl<TextureTarget>)
{
	glGenTextures(1, &d->name);
}

template <int TextureTarget>
Texture<TextureTarget>::Texture(GLuint width, GLuint height, GLuint depth, GLenum flags)
: d(new Impl<TextureTarget>(width,height,depth,flags))
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
GLenum Texture<TextureTarget>::target() const
{
	return TextureTarget;
}


/**
 * Return the name id of the OpenGL object.
 */
template <int TextureTarget>
GLuint Texture<TextureTarget>::oid() const
{
	return d->name;
}

/**
 * Bind the framebuffer object using the target
 */
template <int TextureTarget>
void Texture<TextureTarget>::bind()
{
	glBindTexture(TextureTarget, d->name);
}

/**
 * Unbind the framebuffer
 */
template <int TextureTarget>
void Texture<TextureTarget>::unbind()
{
	glBindTexture(TextureTarget, 0);
}
    
template <int TextureTarget>
void Texture<TextureTarget>::setParameter(GLenum pname, GLint value)
{
	glTexParameteri(TextureTarget, pname, value);
}

// instantiate Texture<GL_TEXTURE_2D>
template class Texture<GL_TEXTURE_2D>;

//
// Texture2D definition
	
Texture2D::Texture2D()
: Texture<GL_TEXTURE_2D>(1,1,1,0)
{
	
}
	
Texture2D::Texture2D(GLuint width, GLuint height, GLenum flags)
: Texture<GL_TEXTURE_2D>(width,height,1,flags)
{
	
}
	
Texture2D::~Texture2D()
{
	
}

void Texture2D::texImage2D(GLenum target,
						  GLint level,
						  GLint internalformat,
						  GLsizei width,
						  GLsizei height,
						  GLint border,
						  GLenum format,
						  GLenum type,
						  const void *data)
{
	glTexImage2D(Texture2D::Target, level, internalformat, width, height, border, format, type, data);
    THROW_IF_GLERROR("Failed to upload texture data");
    d->width =width;
    d->height = height;
    d->depth = 1;
    d->border = border;
    d->format = format;
    d->level = level;
}
	
} /* End gl namespace */
