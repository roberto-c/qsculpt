//
//  CLRender.h
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 7/26/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#ifndef __TestApp__CLRender__
#define __TestApp__CLRender__

#include <vector>
#include <PlastilinaCore/opengl/Texture.h>

class CLRender
{
public:
	CLRender();
	
	~CLRender();
	
	void                                initialize();
	
	void                                shutdown();
	
	std::vector<unsigned char> &       buffer();
	const std::vector<unsigned char> & buffer() const;
	std::vector<unsigned char> &       bufferOutput();
	const std::vector<unsigned char> & bufferOutput() const;
    
    void         						setGLTexSrc(
                                                    gl::Texture2D::shared_ptr tex);
    gl::Texture2D::shared_ptr           glTexSrc();

    void         						setGLTexDest(
                                                    gl::Texture2D::shared_ptr tex);
    gl::Texture2D::shared_ptr           glTexDest();

    
	void								swapBuffers();
	
	void render(float step);
	
private:
	struct Impl;
	Impl * d;
};

#endif /* defined(__TestApp__CLRender__) */
