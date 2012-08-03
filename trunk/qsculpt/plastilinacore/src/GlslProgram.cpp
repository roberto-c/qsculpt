//
//  GlslProgram.cpp
//  qsculpt
//
//  Created by Juan Cabral on 1/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "GlslProgram.h"
#include <iostream>

GlslProgram::GlslProgram() : progId_(0)
{
    progId_ = glCreateProgram();
}

GlslProgram::~GlslProgram()
{
    glDeleteProgram(progId_);
}

GLint GlslProgram::programID() const
{
    return progId_;
}

bool GlslProgram::GlslProgram::link()
{
    glLinkProgram(progId_);
    
    GLint status;
    glGetProgramiv(progId_, GL_LINK_STATUS, &status);
    
    return status == GL_TRUE;
}
    
std::string GlslProgram::buildLog()
{
    GLint val;
    glGetProgramiv(progId_, GL_INFO_LOG_LENGTH, &val);
    
    std::string log;
    
    if (val == 0)
        return log;
    
    std::vector<GLchar> buffer(val);
    try {
        GLsizei length = 0;
        glGetProgramInfoLog(progId_, val, &length, buffer.data());
        
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
    glAttachShader(progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}
   
void GlslProgram::attachShader(FragmentShader * shader)
{
    glAttachShader(progId_, shader->shaderId());
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::useProgram()
{
    glUseProgram(progId_);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::releaseProgram()
{
    glUseProgram(0);
    THROW_IF_GLERROR(__func__);
}

void GlslProgram::bindAttributeLocation(GLuint index, const std::string &name)
{
    glBindAttribLocation(progId_, index, name.c_str());
    THROW_IF_GLERROR(__func__);
}

GLint GlslProgram::uniformLocation(const std::string &name)
{
    GLint loc = glGetUniformLocation(progId_, name.c_str());
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

