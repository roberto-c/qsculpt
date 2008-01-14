#ifndef BUFFEROBJECT_H_
#define BUFFEROBJECT_H_

#include <QtOpenGL>
#include "point3d.h"

class BufferObject
{
public:
	/**
	 * Initializes a Buffer Object (BO)
	 */
	BufferObject();
	~BufferObject();

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
	bool setBufferData(GLfloat* buffer, int size);
	
private:
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

};

#endif /*BUFFEROBJECT_H_*/
