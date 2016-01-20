//
//  Subdivision.cl
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 12/2/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

#include "GpuSubdivisionPrimitives.h"

#define TRACE(msg) printf(msg) 

kernel void surface_init(
    global struct Surface * surface,
    global struct Vertex * vtxList, unsigned int num_vertices,
    global struct Edge * edgeList, unsigned int num_edges,
    global struct Face * faceList, unsigned int num_faces)
{
	surface->num_vertices = num_vertices;
	surface->num_edges = num_edges;
	surface->num_faces = num_faces;
	surface->v.v = vtxList;
	surface->e.e = edgeList;
	surface->f.f = faceList;
	surface->user[0] = 0;
	
	//printf("VtxSize=%d  edgeSize=%d  faceSize=%d\n", num_vertices, num_edges, num_faces);
}



kernel void build_mesh(
	global const struct Surface * inpSurface,
	global float * outVertices
	)
{
	uint faceIdx = get_global_id(0);
	if (faceIdx >= inpSurface->num_faces) {
		return;
	}
	
	uint eIdx = inpSurface->f.f[faceIdx].edge;
	global struct Edge * e = &inpSurface->e.e[eIdx];
	for (int i = 0; i < 3 || e != 0; ++i)
	{
		printf("Face[%d]: edge[%d]\n", faceIdx, e->iid);
		 e = &inpSurface->e.e[e->mext];
	}
}