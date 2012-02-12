//
//  GlslShader.h
//  qsculpt
//
//  Created by Juan Cabral on 1/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_GlslShader_h
#define qsculpt_GlslShader_h

#include <iostream>
#include <fstream>
#include <sstream>

template <GLenum SHADERTYPE>
class GlslShader
{
public:
    GlslShader();
    ~GlslShader();
    
    /**
     * This function return the shader type this instance is.
     */
    GLenum shaderType() const;
    
    /**
     * Return the GL object name or id for this shader.
     */
    GLuint shaderId() const;
    
    /**
     * Load the shader from file
     */
    bool loadFromFile(const std::string & filename);
    
    /**
     * load from a list of strings.
     */
    bool loadFromString(const std::vector<std::string> & source);
    
    /**
     * Compile the shader. A shader should be compiled before use in a  
     * shader program
     *
     * The function compile the shader loaded with loadFromFile or 
     * loadFromString.
     *
     *
     * @returns true if the compilation was successful, false otherwise
     */
    bool compile();
    
    /**
     * Return the a string with compilation log messages.
     *
     * This method should be called after compile() has failed to verify
     */
    std::string infoLog();
    
    /**
     * Return the source code of the shader.
     *
     */
    std::string source() const;
    
protected:
    GLuint                      shaderId_;
};

typedef GlslShader<GL_VERTEX_SHADER> VertexShader;
typedef GlslShader<GL_FRAGMENT_SHADER> FragmentShader;


template <GLenum SHADERTYPE>
GlslShader<SHADERTYPE>::GlslShader() : shaderId_(0)
{
    shaderId_ = glCreateShader(SHADERTYPE);
    if (shaderId_ == 0) {
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
GLenum GlslShader<SHADERTYPE>::shaderType() const
{
    return SHADERTYPE;
}

template <GLenum SHADERTYPE>
GLuint GlslShader<SHADERTYPE>::shaderId() const
{
    return shaderId_;
}

template <GLenum SHADERTYPE>
bool GlslShader<SHADERTYPE>::loadFromFile(const std::string &filename)
{
    std::vector<std::string>    source;
    std::ifstream input(filename.c_str());
    std::vector<char> buffer(1024);
    bool ret = false;
    try {
        while (input.good()) {
            input.getline(buffer.data(), buffer.size());
            source.push_back(buffer.data());
        }
        loadFromString(source);
    } catch (std::exception & e) {
        std::cerr << e.what() << endl;
    }
    ret = true;
    return ret;
}

template <GLenum SHADERTYPE>
bool GlslShader<SHADERTYPE>::loadFromString(const std::vector<std::string> & source)
{
    std::vector<const char*>    lines;
    bool ret = false;
    try {
        size_t size = source.size();
        for (size_t i = 0; i < size; ++i) {
            lines.push_back(source[i].c_str());
        }
        glShaderSource(shaderId_, size, lines.data(), NULL);
    } catch (std::exception & e) {
        std::cerr << e.what() << endl;
    }
    ret = true;
    return ret;
}

template <GLenum SHADERTYPE>
bool GlslShader<SHADERTYPE>::compile() 
{
    assert(glIsShader(shaderId_) == GL_TRUE);
    
    glCompileShader(shaderId_);
    
    GLint val = GL_FALSE;
    glGetShaderiv(shaderId_, GL_COMPILE_STATUS, &val);
    
    return val == GL_TRUE;
}

template <GLenum SHADERTYPE>
std::string GlslShader<SHADERTYPE>::infoLog() 
{
    GLint val;
    glGetShaderiv(shaderId_, GL_INFO_LOG_LENGTH, &val);
    
    std::string log;
    
    if (val == 0)
        return log;
    
    std::vector<GLchar> buffer(val);
    try {
        GLsizei length = 0;
        glGetShaderInfoLog(shaderId_, val, &length, buffer.data());
        
        // Mark the end of buffer with null, just as precaution
        buffer[length] = NULL; 
        log = std::string(buffer.data());
    } catch (std::bad_alloc & e) {
        std::cerr << e.what() << endl;
    }
    
    return log;
}

template <GLenum SHADERTYPE>
std::string GlslShader<SHADERTYPE>::source() const
{
    GLint val;
    glGetShaderiv(shaderId_, GL_SHADER_SOURCE_LENGTH, &val);
    
    std::string src;
    
    if (val == 0)
        return src;
    
    GLchar* buffer = NULL;
    try {
        buffer = new char[val];
        GLsizei length = 0;
        glGetShaderSource(shaderId_, val, &length, buffer);
        
        // Mark the end of buffer with null, just as precaution
        buffer[length] = NULL; 
        src = std::string(buffer);
        
        delete [] buffer;
    } catch (std::bad_alloc & e) {
        std::cerr << e.what() << endl;
    }
    
    return src;
}

#endif
