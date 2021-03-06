//
//  Sampler.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 11/2/13.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/opengl/OpenGL.h>
#include <PlastilinaCore/opengl/Sampler.h>

namespace gl
{

gl::Sampler::Sampler()
    : gl::Object<Sampler>()
{
}

gl::Sampler::~Sampler() {}

void gl::Sampler::bind(GLuint unit) { glBindSampler(unit, oglname_); }

void gl::Sampler::create(uint32_t num, uint32_t* samplers)
{
    glGenSamplers(num, samplers);
}
void gl::Sampler::destroy(uint32_t num, uint32_t* samplers)
{
    glDeleteSamplers(num, samplers);
}
};
