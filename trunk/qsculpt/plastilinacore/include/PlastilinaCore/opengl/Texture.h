//
//  Texture.h
//  qsculpt
//
//  Created by Juan Cabral on 12/27/11.
//  Copyright (c) 2011 plastilinaware. All rights reserved.
//

#ifndef qsculpt_Texture_h
#define qsculpt_Texture_h

namespace gl {
	
class TextureManager
{
	GLenum currentTextureUnit_;
public:
	static TextureManager * instance() {
		static TextureManager * inst = NULL;
		if (!inst) {
			inst = new TextureManager;
		}
		return inst;
	}
	
	GLenum activeTexture() const {
		return currentTextureUnit_;
	}
	void setActiveTexture(GLenum textureUnit) {
		glActiveTexture(currentTextureUnit_);
		currentTextureUnit_ = textureUnit;
	}
	
private:
	TextureManager() : currentTextureUnit_(GL_TEXTURE0)
	{
	}
};

template <int TextureTarget>
class Texture
{
public:
    Texture();

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

	void setInternalFormat(GLint format);
	
	GLint internalFormat() const;
	
protected:
	enum {
		Target = TextureTarget
	};
	
	Texture(GLuint width = 1,
			GLuint height = 1,
			GLuint depth = 1,
			GLenum flags = 0);
	

	template <int TextureTarget2>
	struct Impl;
	std::unique_ptr<Impl<TextureTarget> > d;
};

class Texture2D : public Texture<GL_TEXTURE_2D>
{
public:
	Texture2D();
	Texture2D(GLuint width, GLuint height, GLenum flags);
	virtual ~Texture2D();
	
	void texImage2D(GLenum target,
					GLint level,
					GLint internalformat,
					GLsizei width,
					GLsizei height,
					GLint border,
					GLenum format,
					GLenum type,
					const void *data);
};
}; /* End namespace */

#endif
