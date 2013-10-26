//
//  Subdivision.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 11/25/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

#include "Subdivision.h"
#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>
#include "cl.hpp"


#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/subdivision/Subdivision.h>


void subdivide(SurfaceNode::shared_ptr & node)
{
	try {
		ISurface * s = node->surface();
		Subdivision * surfSubdivision = dynamic_cast<Subdivision*>(s);
		if (surfSubdivision) {
			surfSubdivision->addResolutionLevel();
			BOManager::getInstance()->invalidateBO(s);
		}
	} catch (cl::Error err) {
		std::cerr
		<< "ERROR: "
		<< err.what()
		<< "("
		<< err.err()
		<< ")"
		<< std::endl;
	}
}