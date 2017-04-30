/***************************************************************************
 *   Copyright (C) 2008 by Juan Roberto Cabral Flores                      *
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

#ifndef BUFFEROBJECT_H_
#define BUFFEROBJECT_H_

#include <PlastilinaCore/Point3D.h>
#include <PlastilinaCore/opengl/OpenGL.h>

class DLLEXPORT BufferObject
{
  public:
    BufferObject(const BufferObject& cpy);

    BufferObject(BufferObject&& cpy);
    /**
     *
     */
    bool needUpdate() const { return m_needUpdate; };

    /**
     *
     */
    void setNeedUpdate(bool val) { m_needUpdate = val; };

    /**
     *
     */
    GLuint objectID() const { return m_vboID; };

    /**
     *
     */
    GLenum getType() const { return m_boTarget; };

    /**
     *
     */
    bool mapBuffer(GLvoid** buffer, GLsizeiptr* size);

    /**
     *
     */
    bool unmapBuffer();

    /**
     *
     */
    std::size_t getBufferSize() const { return m_bufferSize; }

    /**
     *
     */
    bool setBufferData(GLvoid* buffer, GLsizeiptr size);

    /**
     *
     */
    bool setBufferSubData(GLint offset, GLsizeiptr size, GLvoid* buffer);

    // private:
    /**
     * Create a vertex buffer object of the specified type.
     */
    bool create();

    /**
     * Deletes the vertex buffer object, freeing all the resources
     */
    bool destroy();

    /**
     * Bind the buffer object to its target.
     */
    bool bind();

    /**
     * Unbind the buffer object.
     */
    bool release();

  protected:
    /**
     * Initializes a Buffer Object (BO)
     */
    BufferObject(GLenum boTarget = 0);

    GLenum     m_boTarget;
    GLuint     m_vboID;      /*< ID of the vertex buffer object */
    GLsizeiptr m_bufferSize; /*< Size of the buffer in bytes? */
    bool       m_needUpdate; /*< True if the data needs to be updated*/
};

inline BufferObject::BufferObject(GLenum boTarget)
    : m_boTarget(boTarget)
    , m_vboID(0)
    , m_bufferSize(0)
    , m_needUpdate(true)
{
}

inline BufferObject::BufferObject(const BufferObject& cpy)
    : m_boTarget(cpy.m_boTarget)
    , m_vboID(cpy.m_vboID)
    , m_bufferSize(cpy.m_bufferSize)
    , m_needUpdate(cpy.m_needUpdate)
{
}

inline BufferObject::BufferObject(BufferObject&& cpy)
    : m_boTarget(std::move(cpy.m_boTarget))
    , m_vboID(std::move(cpy.m_vboID))
    , m_bufferSize(std::move(cpy.m_bufferSize))
    , m_needUpdate(std::move(cpy.m_needUpdate))

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
    bool result = false;

    if (glIsBuffer(m_vboID))
    {
        glDeleteBuffers(1, &m_vboID);
        THROW_IF_GLERROR(__func__);
        RET_ON_GLERROR(result);
        // Reset value
        m_vboID      = 0;
        m_bufferSize = 0;
    }
    result = true;
    return result;
}

inline bool BufferObject::bind()
{
    bool ret = false;

    glBindBuffer(m_boTarget, m_vboID);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(ret);
    ret = true;
    return ret;
}

inline bool BufferObject::release()
{
    bool ret = false;

    glBindBuffer(m_boTarget, 0);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(ret);
    ret = true;
    return ret;
}

/**
 *
 */
inline bool BufferObject::mapBuffer(GLvoid** buffer, GLsizeiptr* size)
{
    bool result = false;

    // Check parameters are not NULL
    if (buffer == NULL || size == NULL)
        return result;

    // Be sure we are working with our buffer
    glBindBuffer(m_boTarget, m_vboID);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    // Set the buffer of the mapped object
    *buffer = glMapBuffer(m_boTarget, GL_READ_WRITE);
    *size   = 0;
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    // If the mapping was successful, update the size value
    if (*buffer != NULL)
    {
        *size = m_bufferSize;
    }
    result = true;
    return result;
}

inline bool BufferObject::unmapBuffer()
{
    bool result = false;

    // Be sure we are working with our buffer
    glBindBuffer(m_boTarget, m_vboID);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    // Unmap the buffer object
    result = glUnmapBuffer(m_boTarget) == GL_TRUE ? true : false;
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    result = true;
    return result;
}

/**
 *
 */
inline bool BufferObject::setBufferData(GLvoid* buffer, GLsizeiptr size)
{
    bool result = false;

    glBindBuffer(m_boTarget, m_vboID);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    m_bufferSize = size;

    glBufferData(m_boTarget, m_bufferSize, buffer, GL_DYNAMIC_DRAW);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    result = true;
    return result;
}

inline bool BufferObject::setBufferSubData(GLint offset, GLsizeiptr size,
                                           GLvoid* buffer)
{
    bool result = false;

    glBindBuffer(m_boTarget, m_vboID);
    THROW_IF_GLERROR(__func__);
    m_bufferSize = size;
    RET_ON_GLERROR(result);

    glBufferSubData(m_boTarget, offset, m_bufferSize, buffer);
    THROW_IF_GLERROR(__func__);
    RET_ON_GLERROR(result);

    result = true;
    return result;
};

class VertexBuffer : public BufferObject
{
  public:
    VertexBuffer()
        : BufferObject(GL_ARRAY_BUFFER)
    {
    }
};

class IndexBuffer : public BufferObject
{
  public:
    IndexBuffer()
        : BufferObject(GL_ELEMENT_ARRAY_BUFFER)
    {
    }
};

#endif /*BUFFEROBJECT_H_*/
