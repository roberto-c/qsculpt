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
	
	Impl(GLuint progId):progId_(progId){}
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
    glLinkProgram(d->progId_);
    
    GLint status;
    glGetProgramiv(d->progId_, GL_LINK_STATUS, &status);
    
	glGetProgramiv(d->progId_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &d->maxNameSize);
    return status == GL_TRUE;
}
    
std::string GlslProgram::buildLog()
{
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
    glAttachShader(d->progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}
   
void GlslProgram::attachShader(FragmentShader * shader)
{
    glAttachShader(d->progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::useProgram()
{
    glUseProgram(d->progId_);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::releaseProgram()
{
    glUseProgram(0);
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::activeAttributes()
{
    GLint res = 0;
    glGetProgramiv(d->progId_, GL_ACTIVE_ATTRIBUTES, &res);
    THROW_IF_GLERROR(__func__);
    return res;
}

void GlslProgram::bindAttributeLocation(GLuint index, const std::string &name)
{
    glBindAttribLocation(d->progId_, index, name.c_str());
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::attributeLocation(const std::string & name)
{
    GLint loc = glGetAttribLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::activeAttrib(GLint index,
                               std::string * name,
                               GLenum * type,
                               GLint * size)
{
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
    GLint loc = glGetFragDataLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::bindFragDataLocation(GLuint colorNumber, const std::string & name)
{
    glBindFragDataLocation(d->progId_, colorNumber, name.c_str());
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::uniformLocation(const std::string &name)
{
    GLint loc = glGetUniformLocation(d->progId_, name.c_str());
    THROW_IF_GLERROR(__func__);
    return loc;
}

void GlslProgram::setUniform(GLint index, int value)
{
    glUniform1iv(index, 1, &value);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector2i & value)
{
    glUniform2iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector3i & value)
{
    glUniform3iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector4i & value)
{
    glUniform4iv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, float value)
{
    glUniform1fv(index, 1, &value);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector2f & value)
{
    glUniform2fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector3f & value)
{
    glUniform3fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Vector4f & value)
{
    glUniform4fv(index, 1, value.data());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::setUniform(GLint index, const Eigen::Matrix4f & val)
{
    glUniformMatrix4fv(index, 1, GL_FALSE, val.data());
}

void GlslProgram::setUniform(GLint index, const Eigen::Affine3f & val)
{
    glUniformMatrix4fv(index, 1, GL_FALSE, val.data());
}

