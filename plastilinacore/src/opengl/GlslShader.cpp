//
//  GlslShader.cpp
//  qsculpt
//
//  Created by Juan Cabral on 2/18/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <fstream>
#include <iostream>

template <GLenum SHADERTYPE>
GlslShader<SHADERTYPE>::GlslShader()
    : shaderId_(0)
{
    shaderId_ = glCreateShader(SHADERTYPE);
    if (shaderId_ == 0)
    {
        throw std::runtime_error("GlslShader: failed to create shader");
    }
}

template <GLenum SHADERTYPE>
GlslShader<SHADERTYPE>::~GlslShader()
{
    glDeleteShader(shaderId_);
    shaderId_ = 0;
}

template <GLenum SHADERTYPE>
GLenum           GlslShader<SHADERTYPE>::shaderType() const
{
    return SHADERTYPE;
}

template <GLenum SHADERTYPE>
GLuint           GlslShader<SHADERTYPE>::shaderId() const
{
    return shaderId_;
}

template <GLenum SHADERTYPE>
bool GlslShader<SHADERTYPE>::loadFromFile(const std::string& filename)
{
    bool                     ret = false;
    std::vector<std::string> source;
    std::ifstream            file(filename);

    std::string line;
    while (file.good())
    {
        getline(file, line);
        line.append("\n");
        source.push_back(line);
    }
    ret = loadFromString(source);
    return ret;
}

template <GLenum SHADERTYPE>
bool             GlslShader<SHADERTYPE>::loadFromString(
    const std::vector<std::string>& source)
{
    std::vector<const char*> lines;
    bool                     ret = false;

    // Check we have code
    if (source.empty())
    {
        return ret;
    }
    try
    {
        GLsizei size = static_cast<GLsizei>(source.size());
        for (GLsizei i = 0; i < size; ++i)
        {
            lines.push_back(source[i].c_str());
        }
        glShaderSource(shaderId_, size, lines.data(), NULL);
        THROW_IF_GLERROR("Failed to set shader source");
        ret = true;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unhandled exception" << std::endl;
    }
    return ret;
}

template <GLenum SHADERTYPE>
bool             GlslShader<SHADERTYPE>::compile()
{
    assert(glIsShader(shaderId_) == GL_TRUE);

    glCompileShader(shaderId_);

    GLint val = GL_FALSE;
    glGetShaderiv(shaderId_, GL_COMPILE_STATUS, &val);

    return val == GL_TRUE;
}

template <GLenum SHADERTYPE>
bool             GlslShader<SHADERTYPE>::compileStatus() const
{
    assert(glIsShader(shaderId_) == GL_TRUE);

    GLint val = GL_FALSE;
    glGetShaderiv(shaderId_, GL_COMPILE_STATUS, &val);

    return val == GL_TRUE;
}

template <GLenum SHADERTYPE>
std::string      GlslShader<SHADERTYPE>::infoLog()
{
    GLint val;
    glGetShaderiv(shaderId_, GL_INFO_LOG_LENGTH, &val);

    std::string log;

    if (val == 0)
        return log;

    std::vector<GLchar> buffer(val);
    try
    {
        GLsizei length = 0;
        glGetShaderInfoLog(shaderId_, val, &length, buffer.data());

        // Mark the end of buffer with null, just as precaution
        buffer[length] = NULL;
        log            = std::string(buffer.data());
    }
    catch (std::bad_alloc& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return log;
}

template <GLenum SHADERTYPE>
std::string      GlslShader<SHADERTYPE>::source() const
{
    GLint val;
    glGetShaderiv(shaderId_, GL_SHADER_SOURCE_LENGTH, &val);

    std::string src;

    if (val == 0)
        return src;

    GLchar* buffer = NULL;
    try
    {
        buffer         = new char[val];
        GLsizei length = 0;
        glGetShaderSource(shaderId_, val, &length, buffer);

        // Mark the end of buffer with null, just as precaution
        buffer[length] = NULL;
        src            = std::string(buffer);

        delete[] buffer;
    }
    catch (std::bad_alloc& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return src;
}

// Instantiate template class.
template class GlslShader<GL_VERTEX_SHADER>;
template class GlslShader<GL_FRAGMENT_SHADER>;
