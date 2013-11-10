//
//  Texture.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 10/18/13.
//
//

#include "Texture.h"

namespace gl {

template <int TextureUnit>
template <int TextureUnit2>
struct Texture<TextureUnit>::Impl {
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

template <int TextureUnit>
Texture<TextureUnit>::Texture() : d(new Impl<TextureUnit>)
{
	glGenTextures(1, &d->name);
}

template <int TextureUnit>
Texture<TextureUnit>::Texture(GLuint width, GLuint height, GLuint depth, GLenum flags)
: d(new Impl<TextureUnit>(width,height,depth,flags))
{
	glGenTextures(1, &d->name);
}

template <int TextureUnit>
Texture<TextureUnit>::~Texture()
{
	glDeleteTextures(1, &d->name);
}

/**
 * get the target to which this framebuffer is going to be bound.
 */
template <int TextureUnit>
GLenum Texture<TextureUnit>::target() const
{
	return TextureUnit;
}


/**
 * Return the name id of the OpenGL object.
 */
template <int TextureUnit>
GLuint Texture<TextureUnit>::oid() const
{
	return d->name;
}

/**
 * Bind the framebuffer object using the target
 */
template <int TextureUnit>
void Texture<TextureUnit>::bind()
{
	glBindTexture(TextureUnit, d->name);
}

/**
 * Unbind the framebuffer
 */
template <int TextureUnit>
void Texture<TextureUnit>::unbind()
{
	glBindTexture(TextureUnit, 0);
}

	
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
}
	
} /* End gl namespace */
