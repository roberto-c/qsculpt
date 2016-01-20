//
//  Subdivision.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 11/25/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//
#include "stable.h"
#include "Subdivision.h"

#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/opencl/OpenCL.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>
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
	} catch (cl::Error e) {
        TRACE(error) << "OpenCL exception:" << e.err() << " (" << core::cl::errorToString(e.err()) << "): " << e.what();
	}
}