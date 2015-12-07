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
	
class DLLEXPORT TextureManager
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
class DLLEXPORT Texture
{
public:
	typedef std::shared_ptr<Texture<TextureTarget> > shared_ptr;
	typedef std::weak_ptr<Texture<TextureTarget> > weak_ptr;
	typedef std::shared_ptr<const Texture<TextureTarget> > const_shared_ptr;
	typedef std::weak_ptr<const Texture<TextureTarget> > const_weak_ptr;
	
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

	void setInternalFormat(GLint format, GLenum pname, GLint value);
	
	GLint internalFormat(GLint format, GLenum pname) const;
	
    void setParameter(GLenum pname, GLint value);
    
protected:
	enum {
		Target = TextureTarget
	};
	
	Texture(GLuint width,
			GLuint height,
			GLuint depth,
			GLenum flags);
	

	template <int TextureTarget2>
	struct Impl;
	std::unique_ptr<Impl<TextureTarget> > d;
};

class DLLEXPORT Texture2D : public Texture<GL_TEXTURE_2D>
{
public:
    typedef std::shared_ptr<Texture2D> shared_ptr;
	typedef std::weak_ptr<Texture2D> weak_ptr;
	typedef std::shared_ptr<const Texture2D> const_shared_ptr;
	typedef std::weak_ptr<const Texture2D> const_weak_ptr;
	
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
}; /* End namespace gl*/

#endif
