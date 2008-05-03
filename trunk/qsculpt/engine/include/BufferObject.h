#ifndef BUFFEROBJECT_H_
#define BUFFEROBJECT_H_

#include <QtOpenGL>
#include "Point3D.h"

template <GLenum boTarget>
class BufferObject
{
public:
	/**
	 * Initializes a Buffer Object (BO)
	 */
	BufferObject();
	
	/**
	 * Destroy or frees the resources used by the buffer object
	 */
	~BufferObject();

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
		return boTarget;
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
	int getBufferSize();

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
	
private:
	GLuint		m_vboID;	/*< ID of the vertex buffer object */
	GLuint		m_bufferSize; /*< Size of the buffer in bytes? */
	GLfloat*	m_buffer;	/*< pointer to a buffer containing the vertex
	 						    buffer object data */
	bool		m_needUpdate; /*< True if the data needs to be updated*/
};

template <GLenum boTarget>
BufferObject<boTarget>::BufferObject()
:	m_vboID(0),
m_bufferSize(0),
m_buffer(NULL),
m_needUpdate(true)
{
	//create();
}

template <GLenum boTarget>
BufferObject<boTarget>::~BufferObject()
{
	//destroy();
}

/**
 * Create a vertex buffer object of the specified type.
 */
template <GLenum boTarget>
bool BufferObject<boTarget>::create()
{
	glGenBuffers(1, &m_vboID);
	return true;
}

/**
 * Deletes the vertex buffer object, freeing all the resources
 */
template <GLenum boTarget>
bool BufferObject<boTarget>::destroy()
{
	GLuint error = GL_NO_ERROR;
	
	if (glIsBuffer(m_vboID))
	{
		glDeleteBuffers(1, &m_vboID);
		if ((error = glGetError()) != GL_NO_ERROR) goto Error;
		
		// Reset value
		m_vboID = 0;
		m_buffer = NULL;
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
template <GLenum boTarget>
bool BufferObject<boTarget>::mapBuffer(GLvoid** buffer, GLuint* size)
{
	GLuint error = GL_NO_ERROR;
	
	// Check parameters are not NULL
	if (buffer == NULL || size == NULL)
		return false;
	
	// Be sure we are working with our buffer
	glBindBuffer(boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	// Set the buffer of the mapped object
	*buffer = glMapBuffer(boTarget, GL_READ_WRITE);
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

template <GLenum boTarget>
bool BufferObject<boTarget>::unmapBuffer()
{
	GLuint error = GL_NO_ERROR;
	bool result = true;
	
	// Be sure we are working with our buffer
	glBindBuffer(boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	// Unmap the buffer object
	result = glUnmapBuffer(boTarget);
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
template <GLenum boTarget>
int BufferObject<boTarget>::getBufferSize()
{
	return m_bufferSize;
}

/**
 *
 */
template <GLenum boTarget>
bool BufferObject<boTarget>::setBufferData(GLvoid* buffer, GLuint size)
{
	GLuint error = GL_NO_ERROR;
	
	glBindBuffer(boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	m_bufferSize = size;
	
	glBufferData(boTarget, 
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

template <GLenum boTarget>
bool BufferObject<boTarget>::setBufferSubData(GLint offset, GLuint size, GLvoid* buffer)
{
	GLuint error = GL_NO_ERROR;
	
	glBindBuffer(boTarget, m_vboID);
	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
	
	m_bufferSize = size;
	
	glBufferSubData(boTarget,
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
}
typedef BufferObject<GL_ARRAY_BUFFER> VertexBuffer;
typedef BufferObject<GL_ELEMENT_ARRAY_BUFFER> IndexBuffer;

#endif /*BUFFEROBJECT_H_*/
