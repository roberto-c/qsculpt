#ifndef VBO_H_
#define VBO_H_

#include <QtOpenGL>
class Vertex;

class VBO
{
public:
	VBO();
	~VBO();

	/**
	 * Create a vertex buffer object of the specified type.
	 */
	bool create(GLuint type);

	/**
	 * Deletes the vertex buffer object, freeing all the resources
	 */
	bool destroy();

	/**
	 *
	 */
	GLfloat* getBufferPointer();

	/**
	 *
	 */
	int getBufferSize();

	/**
	 *
	 */
	bool setBufferData(GLfloat* buffer, int size);

	/**
	 *
	 */
	Vertex* getVertexPointer();

	/**
	 *
	 */
	int getVertexCount();
private:
	GLuint		m_vboID;	/*< ID of the vertex buffer object */
	GLuint		m_bufferSize; /*< Size of the buffer in bytes? */
	GLfloat*	m_buffer;	/*< pointer to a buffer containing the vertex
	 						    buffer object data */

};

#endif /*VBO_H_*/
