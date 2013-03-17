//
//  GlslProgram.h
//  qsculpt
//
//  Created by Juan Cabral on 1/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_GlslProgram_h
#define qsculpt_GlslProgram_h

#include <PlastilinaCore/opengl/GlslShader.h>

class GlslProgram
{
public:
	/**
	 * Static method to return the active shader program.
	 */
	static GlslProgram * currentProgram();
	
    GlslProgram();
    
    ~GlslProgram();
    
    /**
     * Name or id of the shader program as known to OpenGL.
     */
    GLuint programID() const;
    
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
     * Get the number of active attributes for this program.
     *
     * This is meant to be called after the program has been linked, once that
     * the compiler / linker can determine which attributes declared in the
     * shader(s) source are used.
     *
     * @return number of attributes used in the program.
     */
    GLint activeAttributes();
    
    /**
     * Bind attribute name to the program
     */
    void bindAttributeLocation(GLuint index, const std::string & name);
    
    /**
     * Returns the attribute location for a given attribute name.
     *
     * @param name name of the attribute
     *
     * @return the location index of attribute. -1 if an error is encountered.
     */
    GLint attributeLocation(const std::string & name);
    
    /**
     * Returns informations about a given active attribute.
     *
     * @param index index of the attribute. Must be between 0 and 
     * ACTIVE_ATTRIBUTES.
     * @param name string object to hold the name into. It is optional, passing
     * NULL in this parameter will not return the name of the attribute.
     * @param type pointer to an enum to store the OpenGL type for the attribute.
     * This is optional, if NULL, then no attempt to get the type is done.
     * @param size size for the type returned in units of the given type.
     */
    void activeAttrib(GLint index,
                      std::string * name,
                      GLenum * type,
                      GLint * size);
    
    /**
     * Get the color number location to which an output variable is bound.
     */
    GLint fragDataLocation(const std::string & name);
    
    /**
     * Bind output variable with name for fragment shaders to the colorNumber 
     * specified.
     *
     * @param colorNumber color number to bind the variable
     * @param name name of the variable to bind.
     */
    void bindFragDataLocation(GLuint colorNumber, const std::string & name);
    
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
    
    /**
     * Set the value of uniform variable at location index.
     *
     * The location is obtained by calling the function uniformLocation()
     *
     * @param index location index of uniform to set the value.
     * @param val value to set.
     */
    void setUniform(GLint index, const Eigen::Matrix4f & val);
    
private:
	GlslProgram(GLuint pid);
	void setProgramID(GLuint pid);
	
	struct Impl;
	std::unique_ptr<Impl> d;
};

#endif
