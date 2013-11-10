//
//  Sampler.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 11/2/13.
//
//

#ifndef __PlastilinaCore__Sampler__
#define __PlastilinaCore__Sampler__

#include <iostream>
#include <PlastilinaCore/opengl/Object.h>

namespace gl {
// OpenGL Sampler object wrapper
	class Sampler : public gl::Object<Sampler> {
	Sampler();
	~Sampler();
	
	void bind();
	
protected:
	void create(uint num, uint & name);
	void destroy();
};
};


#endif /* defined(__PlastilinaCore__Sampler__) */
