//
//  GlslProgram.cpp
//  qsculpt
//
//  Created by Juan Cabral on 1/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <iostream>


struct GlslProgram::Impl
{
    GLuint 	progId_;
	GLint	maxNameSize;
    bool    compiled;
	
	Impl(GLuint progId)
        : progId_(progId)
        , compiled(false)
    {}
};

GlslProgram * GlslProgram::currentProgram()
{
	static GlslProgram prog;
	GLint pid;
	glGetIntegerv(GL_CURRENT_PROGRAM,&pid);
	prog.setProgramID(pid);
	return &prog;
}

GlslProgram::GlslProgram() : d(new Impl(0))
{
    d->progId_ = glCreateProgram();
    if (d->progId_ == 0) {
        throw core::GlException("Failed to create program", 0);
    }
}

GlslProgram::GlslProgram(GLuint pid) : d(new Impl(0))
{
	if (glIsProgram(pid)) {
		d->progId_ = pid;
	}
}

GlslProgram::~GlslProgram()
{
    glDeleteProgram(d->progId_);
}

GLuint GlslProgram::programID() const
{
    return d->progId_;
}

void GlslProgram::setProgramID(GLuint pid)
{
    d->progId_ = pid;
}

bool GlslProgram::GlslProgram::link()
{
    assert(d->progId_);
    glLinkProgram(d->progId_);
    
    GLint status;
    glGetProgramiv(d->progId_, GL_LINK_STATUS, &status);
    
	glGetProgramiv(d->progId_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &d->maxNameSize);
    d->compiled = (status == GL_TRUE);
    return d->compiled;
}

std::string GlslProgram::buildLog()
{
    assert(d->progId_);
    GLint val;
    glGetProgramiv(d->progId_, GL_INFO_LOG_LENGTH, &val);
    
    std::string log;
    
    if (val == 0)
        return log;
    
    std::vector<GLchar> buffer(val);
    try {
        GLsizei length = 0;
        glGetProgramInfoLog(d->progId_, val, &length, buffer.data());
        
        // Mark the end of buffer with null, just as precaution
        buffer[length] = NULL; 
        log = std::string(buffer.data());
    } catch (std::bad_alloc & e) {
        std::cerr << e.what() << std::endl;
    }
    THROW_IF_GLERROR(__func__);
    
    return log;
}
    
void GlslProgram::attachShader(VertexShader * shader)
{
    assert(d->progId_);
    glAttachShader(d->progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}
   
void GlslProgram::attachShader(FragmentShader * shader)
{
    assert(d->progId_);
    glAttachShader(d->progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::useProgram()
{
    assert(d->progId_);
    glUseProgram(d->progId_);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::releaseProgram()
{
    assert(d->progId_);
    glUseProgram(0);
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::activeAttributes()
{
    assert(d->progId_);
    GLint res = 0;
    glGetProgramiv(d->progId_, GL_ACTIVE_ATTRIBUTES, &res);
    THROW_IF_GLERROR(__func__);
    return res;
}

void GlslProgram::bindAttributeLocation(GLuint index, const std::string &name)
{
    assert(d->progId_);
    glBindAttribLocation(d->progId_, index, name.c_str());
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::attributeLocation(const std::string & name)
{
    assert(d->progId_);
    GLint loc = glGetAttribLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::activeAttrib(GLint index,
                               std::string * name,
                               GLenum * type,
                               GLint * size)
{
    assert(d->progId_);
    if (!name && !type) {
        return;
    }

    std::vector<GLchar> buffer;
	buffer.resize(d->maxNameSize);
	GLsizei bufSize = static_cast<GLsizei>(buffer.size());
    glGetActiveAttrib(d->progId_, index, bufSize, NULL, size, type, buffer.data());
    THROW_IF_GLERROR(__func__);
    if (name) {
        name->assign(buffer.begin(),buffer.end());
    }
}

GLint GlslProgram::fragDataLocation(const std::string & name)
{
    assert(d->progId_);
    GLint loc = glGetFragDataLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::bindFragDataLocation(GLuint colorNumber, const std::string & name)
{
    assert(d->progId_);
    glBindFragDataLocation(d->progId_, colorNumber, name.c_str());
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::uniformLocation(const std::string &name)
{
    assert(d->progId_);
    GLint loc = glGetUniformLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::setUniform(GLint index, int value)
{
    assert(d->progId_);
    glUniform1iv(index, 1, &value);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector2i & value)
{
    assert(d->progId_);
    glUniform2iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector3i & value)
{
    assert(d->progId_);
    glUniform3iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector4i & value)
{
    assert(d->progId_);
    glUniform4iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, float value)
{
    assert(d->progId_);
    glUniform1fv(index, 1, &value);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector2f & value)
{
    assert(d->progId_);
    glUniform2fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector3f & value)
{
    assert(d->progId_);
    glUniform3fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector4f & value)
{
    assert(d->progId_);
    glUniform4fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Matrix4f & val)
{
    assert(d->progId_);
    glUniformMatrix4fv(index, 1, GL_FALSE, val.data());
}

void GlslProgram::setUniform(GLint index, const Eigen::Affine3f & val)
{
    assert(d->progId_);
    glUniformMatrix4fv(index, 1, GL_FALSE, val.data());
}

