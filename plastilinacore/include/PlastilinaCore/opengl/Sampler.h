//
//  Sampler.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 11/2/13.
//
//

#ifndef __PlastilinaCore__Sampler__
#define __PlastilinaCore__Sampler__

#include <PlastilinaCore/opengl/Object.h>
#include <array>
#include <iostream>

class Color;

namespace gl
{
// OpenGL Sampler object wrapper
class Sampler : public gl::Object<Sampler>
{
    Sampler();
    ~Sampler();

    void bind(GLuint unit);

    void setTextureBorderColor(const Color& c);
    Color textureBorderColor() const;

    void setTextureMinFilter(Eigen::Vector2f& params);
    std::array<GLint, 6> textureMinFilter() const;

    void setTextureMagFilter(const std::array<GLint, 6>& params);
    std::array<GLint, 6> textureMagFilter() const;

    void setParameter(GLuint paramName, void* value);

  public:
    static void create(GLuint num, GLuint* name);
    static void destroy(GLuint num, GLuint* names);
};
};

#endif /* defined(__PlastilinaCore__Sampler__) */
