//
//  GlslProgram.h
//  qsculpt
//
//  Created by Juan Cabral on 1/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_GlslProgram_h
#define qsculpt_GlslProgram_h

#include "GlslShader.h"

class GlslProgram
{
public:
    GlslProgram();
    
    ~GlslProgram();
    
    /**
     * Name or id of the shader program as known to OpenGL.
     */
    GLint programID() const;
    
    /**
     *
     */
    bool link();
    
    /**
     *
     */
    std::string buildLog();
    
    /**
     * Add the shader to the current program.
     *
     * @param shader shader to add to the program.
     */
    void attachShader(VertexShader * shader);
    
    /**
     * Add the shader to the current program.
     *
     * @param shader shader to add to the program.
     */
    void attachShader(FragmentShader * shader);
    
    /**
     * Binds this program to the current context.
     *
     * Equivalent to calling glUseProgram(). The program has to be compiled,
     * otherwise it will throw an error.
     */
    void useProgram();
    
    /**
     * Makes the shader program to be no longer used. Switching to fixed
     * pipeline.
     *
     * Equivalent to calling glUseProgram(0).
     */
    void releaseProgram();
    
    /**
     * Bind attribute name to the program
     */
    void bindAttributeLocation(GLuint index, const std::string & name);
    
    /**
     * Get the location of a uniform variable.
     *
     * Used after the program has been linked successfully, to retrieve the 
     * location of the uniform variable and be able to set a value to it.
     *
     * @param name name of the uniform variable to look for.
     *
     * @return the index of uniform in the program. -1 if there is an error.
     */
    GLint uniformLocation(const std::string & name);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, int val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector2i & val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector3i & val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector4i & val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, float val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector2f & val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector3f & val);
    
    /**
     * Set the value of uniform variable at location index. 
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Vector4f & val);
private:
    GLuint progId_;
};

#endif
