//
//  ParticleSystem.cpp
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 1/21/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

#include "ParticleSystem.h"
#define __CL_ENABLE_EXCEPTIONS
#include <OpenCL/opencl.h>
#include "cl.hpp"


#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/opencl/OCLManager.h>
#include <PlastilinaCore/opencl/CLUtils.h>

struct clSurfaceList;

struct Impl {
    /** Instance ID of the surface */
    size_t _iid;
	
	std::unordered_map<uint, uint> hndVtxIndex;
	std::unordered_map<uint, uint> hndEdgeIndex;
	std::unordered_map<uint, uint> hndFaceIndex;
	bool 				oclInitialized;
	cl::Program 		program;
	cl::Kernel			krnInit;
	cl::Kernel 			krnSubdivideEdges;
	cl::Kernel 			krnSubdivideFaces;
	cl::Kernel			krnSubdivideAddFaces;
    
    Impl() : oclInitialized(false)
    {
    }
    
	int 		initialize_ocl(void);
	
	void 		step(Subdivision * s);
	
	struct clParticleSystem	create_from_surface(Subdivision * surface);
	void 					update_surface_from_polygon(Subdivision * surface,
														clParticleSystem & list);
};


int
Impl::initialize_ocl(void)
{
	cl_int err = CL_SUCCESS;
	
	try {
		CLManager * oclManager = CLManager::instance();
		
		std::string kernelSource = opencl::loadFromFile("/Users/rcabral/Projects/qsculpt/qsculpt/testapp/ParticleSystem.cl");
		cl::Program::Sources source(1,
									std::make_pair(kernelSource.c_str(),kernelSource.length()));
		program = cl::Program(oclManager->context(), source);
		program.build(oclManager->devices());
		
		krnInit = cl::Kernel(program,"surface_init",&err);
		krnSubdivideEdges = cl::Kernel(program, "subdivide_edges_midpoint", &err);
		krnSubdivideFaces = cl::Kernel(program, "subdivide_faces_midpoint", &err);
		krnSubdivideAddFaces = cl::Kernel(program, "subdivide_add_faces", &err);
	}
	catch (cl::Error err) {
		std::cerr
		<< "ERROR: "
		<< err.what()
		<< "("
		<< err.err()
		<< ")"
		<< std::endl;
	}
	
	return EXIT_SUCCESS;
}

struct clParticle {
	cl_float4 x;
	cl_float4 v;
	cl_float4 x0;
	cl_float4 f;
};

struct clParticleSystem {
	cl_uint 				num_particles;
	std::vector<clParticle> p;
};

struct clParticleSystem
Impl::create_from_surface(Subdivision * surface) {
	struct clParticleSystem ret;
	
	if (!surface) {
		ret.num_particles = 0;
		return ret;
	}
	
	ret.p.reserve(1000);
	auto vtxIt = surface->constVertexIterator();
	while (vtxIt.hasNext()) {
		auto v = static_cast<Vertex*>(vtxIt.next());
		if (!v) {
			throw std::runtime_error("Invalid vertex in surface");
		}
		
		struct clParticle vtx;
		vtx.x.s[0] = v->position()[0];
		vtx.x.s[1] = v->position()[1];
		vtx.x.s[2] = v->position()[2];
		vtx.x.s[3] = 1;
		vtx.v.s[0] = 0.01f;
		vtx.v.s[1] = 0.01f;
		vtx.v.s[2] = 0.01f;
		vtx.v.s[3] = 0;
		ret.p.push_back(vtx);
		hndVtxIndex[v->iid()] = uint(ret.p.size()) - 1;
	}

	return ret;
}

void
Impl::update_surface_from_polygon(Subdivision * surface,
											   clParticleSystem & list)
{
	if (!surface) {
		return;
	}

	
	std::vector<uint> vtxIdxIid(hndVtxIndex.size());
	
	for (uint i = 0; i < list.p.size(); ++i) {
		Point3 p(list.p[i].x.s[0],list.p[i].x.s[1],list.p[i].x.s[2]);
		Vertex::size_t iid = surface->addVertex(p);
		hndVtxIndex[iid] = i;
		vtxIdxIid[i] = iid;
		std::cout << "Point[" << i << "]=" << toString(p) << std::endl;
	}
	//surface->vertex()
	
	surface->setChanged(true);
}

void Impl::step(Subdivision * s)
{
	if (!oclInitialized) {
		initialize_ocl();
		oclInitialized = true;
	}
	
	
	try {
		CLManager * oclManager = CLManager::instance();
		cl::Context context = oclManager->context();
		cl::CommandQueue queue = oclManager->commandQueue();
		
		
		clParticleSystem surface1 = create_from_surface(s);
		std::cout << "Vertex size: " << surface1.p.size() << std::endl;
		
		cl::Buffer surfaceBuffer1(context,CL_MEM_READ_WRITE, sizeof(clParticleSystem), 0);
		
		unsigned long vertexBufferSize1 = surface1.p.capacity()*sizeof(clParticle);
		cl::Buffer vertexBuffer1(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vertexBufferSize1,surface1.p.data());
				
		clParticleSystem surface2 = create_from_surface(s);
		
		cl::Buffer surfaceBuffer2(context,CL_MEM_READ_WRITE, sizeof(clParticleSystem), 0);
		
		unsigned long vertexBufferSize2 = surface2.p.capacity()*sizeof(clParticle);
		cl::Buffer vertexBuffer2(context,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vertexBufferSize2,surface2.p.data());
				
		krnInit.setArg(0, surfaceBuffer1);
		krnInit.setArg(1, vertexBuffer1);
		krnInit.setArg(2, surface1.p.size());
		queue.enqueueTask(krnInit,NULL,NULL);
		
		krnInit.setArg(0, surfaceBuffer2);
		krnInit.setArg(1, vertexBuffer2);
		krnInit.setArg(2, surface2.p.size());
		cl::Event evtInit;
		queue.enqueueTask(krnInit,NULL,&evtInit);
		
		krnSubdivideEdges.setArg(0, surfaceBuffer1);
		krnSubdivideEdges.setArg(1, surfaceBuffer2);
		
		std::vector<cl::Event> evtLst;
		evtLst.push_back(evtInit);
		cl::Event event;
		queue.enqueueNDRangeKernel(
								   krnSubdivideEdges,
								   cl::NullRange,
								   cl::NDRange(s->numEdges()/2),
								   cl::NullRange,
								   &evtLst,
								   &event);
		
		
		update_surface_from_polygon(s, surface2);
		
		//		BOManager::getInstance()->invalidateBO(s);
		
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

static void
step(ISurface * s, float time)
{
	auto vtxIt = s->vertexIterator();
	while (vtxIt.hasNext()) {
		auto vtx = static_cast<Vertex*>(vtxIt.next());
		vtx->position() += Vector3(0.01f,0.01f,0.01f);
	}
}

void ParticleSystem_step(SurfaceNode::shared_ptr & node, float time)
{
	std::cout << "ParticleSystem_step" << std::endl;
	try {
		ISurface * s = node->surface();
		if (!s)
			return;
		step(s, time);
		BOManager::getInstance()->invalidateBO(s);
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
