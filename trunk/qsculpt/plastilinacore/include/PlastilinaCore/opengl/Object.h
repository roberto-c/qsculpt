//
//  Object.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 11/3/13.
//
//

#ifndef PlastilinaCore_Object_h
#define PlastilinaCore_Object_h

namespace gl {
template<typename T>
class Object {
protected:
	uint oglname_;
	
public:
	Object() : oglname_(0) {
		T::create(1,&oglname_);
	}
	
	~Object() {
		T::destroy(oglname_);
	}
};
};

#endif
