#ifndef BUFFEROBJECT_H_
#define BUFFEROBJECT_H_

#include <QtOpenGL>
#include "Point3D.h"

class BufferObject
{
public:		
	/**
	 *
	 */
	bool needUpdate() const {
		return m_needUpdate;
	};
	
	/**
	 *
	 */
	void setNeedUpdate(bool val) {
		m_needUpdate = val;
	};
	
	/**
	 *
	 */
	GLuint getBufferID() const {
		return m_vboID;
	};
	
	/**
	 *
	 */
	GLenum getType() const {
		return m_boTarget;
	};
	
	/**
	 *
	 */
	bool mapBuffer(GLvoid** buffer, GLuint* size);
	
	/**
	 *
	 */
	bool unmapBuffer();
	
	/**
	 *
	 */
	int getBufferSize() const {
		return m_bufferSize;
	}
	
	/**
	 *
	 */
	bool setBufferData(GLvoid* buffer, GLuint size);
	
	/**
	 *
	 */
	bool setBufferSubData(GLint offset, GLuint size, GLvoid* buffer);
	
	//private:
	/**
	 * Create a vertex buffer object of the specified type.
	 */
	bool create();
	
	/**
	 * Deletes the vertex buffer object, freeing all the resources
	 */
	bool destroy();
	
protected:
	/**
	 * Initializes a Buffer Object (BO)
	 */
	BufferObject(GLenum boTarget = 0);
	
	GLenum		m_boTarget;
	GLuint		m_vboID;		/*< ID of the vertex buffer object */
	GLuint		m_bufferSize;	/*< Size of the buffer in bytes? */
	bool		m_needUpdate;	/*< True if the data needs to be updated*/
};

inline BufferObject::BufferObject(GLenum boTarget)
:	m_boTarget(boTarget),
	m_vboID(0),
	m_bufferSize(0),
	m_needUpdate(true)
{
}

/**
 * Create a vertex buffer object of the specified type.
 */
inline bool BufferObject::create()
{
	glGenBuffers(1, &m_vboID);
	return true;
}

/**
 * Deletes the vertex buffer object, freeing all the resources
 */
inline bool BufferObject::destroy()
{
	GLuint error = GL_NO_ERROR;
	
	if (glIsBuffer(m_vboID))
	{
		glDeleteBuffers(1, &m_vboID);
		if ((error = glGetError()) != GL_NO_ERROR) goto Error;
		
		// Reset value
		m_vboID = 0;
		m_bufferSize = 0;
	}
	
Error:
	bool result = (error == GL_NO_ERROR);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}


/**
 *
 */
inline bool BufferObject::mapBuffer(GLvoid** buffer, GLuint* size)
{
	GLuint error = GL_NO_ERROR;
	
	// Check parameters are not NULL
	if (buffer == NULL || size == NULL)
		return false;
	
	// Be sure we are working with our buffer
	glBindBuffer(m_boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	// Set the buffer of the mapped object
	*buffer = glMapBuffer(m_boTarget, GL_READ_WRITE);
	*size = 0;
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	// If the mapping was successful, update the size value
	if (*buffer != NULL)
	{
		*size = m_bufferSize;
	}
	
Error:
	bool result = (error == GL_NO_ERROR) && (buffer != NULL);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}

inline bool BufferObject::unmapBuffer()
{
	GLuint error = GL_NO_ERROR;
	bool result = true;
	
	// Be sure we are working with our buffer
	glBindBuffer(m_boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	// Unmap the buffer object
	result = glUnmapBuffer(m_boTarget);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
Error:
	result = result && (error == GL_NO_ERROR);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}

/**
 *
 */
inline bool BufferObject::setBufferData(GLvoid* buffer, GLuint size)
{
	GLuint error = GL_NO_ERROR;
	
	glBindBuffer(m_boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	m_bufferSize = size;
	
	glBufferData(m_boTarget, 
				 m_bufferSize,
				 buffer,
				 GL_DYNAMIC_DRAW);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
Error:
	bool result = (error == GL_NO_ERROR);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
}

inline bool BufferObject::setBufferSubData(GLint offset, GLuint size, GLvoid* buffer)
{
	GLuint error = GL_NO_ERROR;
	
	glBindBuffer(m_boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	m_bufferSize = size;
	
	glBufferSubData(m_boTarget,
					offset,
					m_bufferSize,
					buffer);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
Error:
	bool result = (error == GL_NO_ERROR);
	for(;error != GL_NO_ERROR; error = glGetError())
	{
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
	}
	return result;
};

class VertexBuffer : public BufferObject
{
public:
	VertexBuffer() : BufferObject(GL_ARRAY_BUFFER) {}
};

class IndexBuffer : public BufferObject
{
public:
	IndexBuffer() : BufferObject(GL_ELEMENT_ARRAY_BUFFER) {}
};

#endif /*BUFFEROBJECT_H_*/
