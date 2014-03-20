//
//  Sampler.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 11/2/13.
//
//

#include "PlastilinaCore/opengl/Sampler.h"


namespace gl {

gl::Sampler::Sampler()
	: gl::Object<Sampler>()
{

}
	
gl::Sampler::~Sampler()
{

}
		
void gl::Sampler::bind(GLuint unit)
{
	glBindSampler(unit, oglname_);
}
		
		
void gl::Sampler::create(uint num, uint * samplers)
{
	glGenSamplers(num, samplers);
}
void gl::Sampler::destroy(uint num, uint * samplers)
{
	glDeleteSamplers(num, samplers);
}
	
};
