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
#include <vector>
#include <string>
#include <PlastilinaCore/opengl/OpenGL.h>

template <GLenum SHADERTYPE>
class DLLEXPORT GlslShader
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
     * Returns the status of the compilation.
     *
     * @returns true if the previous call to compile was successful; 
     * false, otherwise.
     */
    bool compileStatus() const;
    
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


#endif
