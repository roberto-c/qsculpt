//
//  ParticleSystem.cl
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 1/27/13.
//  Copyright (c) 2013 Juan Roberto Cabral Flores. All rights reserved.
//

struct Particle {
	float4 x;
	float4 v;
	float4 x0;
	float4 f;
};

struct ParticleSystem {
	uint 						num_particles;
	__global struct Particle * 	p;
};

__kernel void ParticleSystem_init(__global struct ParticleSystem * system,
								  __global struct Particle * p)
{
	printf("ParticleSystem_init");
//	system->p = p;
}

__kernel void compute_forces(__global struct ParticleSystem * system, float time)
{
	
}

__kernel void do_step(__global struct ParticleSystem * system, float time)
{
	uint index = get_global_id(0);
	
	system->p->x += system->p->v;
}
