//#include "../include/BufferObject.h"

//BufferObject::BufferObject()
//:	m_vboID(0),
//	m_bufferSize(0),
//	m_buffer(NULL)
//{
//	create();
//}
//
//BufferObject::~BufferObject()
//{
//	destroy();
//}
//
///**
// * Create a vertex buffer object of the specified type.
// */
//bool BufferObject::create()
//{
//	glGenBuffers(1, &m_vboID);
//	return true;
//}
//
///**
// * Deletes the vertex buffer object, freeing all the resources
// */
//bool BufferObject::destroy()
//{
//	GLuint error = GL_NO_ERROR;
//	
//	if (glIsBuffer(m_vboID))
//	{
//		glDeleteBuffers(1, &m_vboID);
//		if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//		
//		// Reset value
//		m_vboID = 0;
//		m_buffer = NULL;
//		m_bufferSize = 0;
//	}
//	
//Error:
//	bool result = (error == GL_NO_ERROR);
//	for(;error != GL_NO_ERROR; error = glGetError())
//	{
//		const GLubyte* strError = gluErrorString(error);
//		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
//	}
//	return result;
//}
//
///**
// *
// */
//bool BufferObject::mapBuffer(GLvoid** buffer, GLuint* size)
//{
//	GLuint error = GL_NO_ERROR;
//	
//	// Check parameters are not NULL
//	if (buffer == NULL || size == NULL)
//		return false;
//	
//	// Be sure we are working with our buffer
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//	// Set the buffer of the mapped object
//	*buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
//	*size = 0;
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//	// If the mapping was successful, update the size value
//	if (*buffer != NULL)
//	{
//		*size = m_bufferSize;
//	}
//	
//Error:
//	bool result = (error == GL_NO_ERROR) && (buffer != NULL);
//	for(;error != GL_NO_ERROR; error = glGetError())
//	{
//		const GLubyte* strError = gluErrorString(error);
//		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
//	}
//	return result;
//}
//
//bool BufferObject::unmapBuffer()
//{
//	GLuint error = GL_NO_ERROR;
//	
//	// Be sure we are working with our buffer
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//	// Unmap the buffer object
//	bool result = glUnmapBuffer(GL_ARRAY_BUFFER);
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//Error:
//	result = result && (error == GL_NO_ERROR);
//	for(;error != GL_NO_ERROR; error = glGetError())
//	{
//		const GLubyte* strError = gluErrorString(error);
//		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
//	}
//	return result;
//}
//
///**
// *
// */
//int BufferObject::getBufferSize()
//{
//	return m_bufferSize;
//}
//
///**
// *
// */
//bool BufferObject::setBufferData(GLfloat* buffer, int size)
//{
//	GLuint error = GL_NO_ERROR;
//	
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//	m_bufferSize = size;
//	
//	glBufferData(GL_ARRAY_BUFFER, 
//				 m_bufferSize,
//				 (GLvoid*)buffer,
//				 GL_DYNAMIC_DRAW);
//	if ((error = glGetError()) != GL_NO_ERROR) goto Error;
//	
//Error:
//	bool result = (error == GL_NO_ERROR);
//	for(;error != GL_NO_ERROR; error = glGetError())
//	{
//		const GLubyte* strError = gluErrorString(error);
//		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
//	}
//	return result;
//}

