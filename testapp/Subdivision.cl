//
//  Subdivision.cl
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 12/2/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

struct Vertex {
	float4 	p;	// position
	float4 	n;	// notmal
	uint		e;	// edge index
};

struct Edge {
	uint	head;	// index to vertex array
	uint 	pair; 	// index to edge array
	uint 	face;	// index to a face
	uint 	next;	// next edge when forming a face
//	uint 	midpoint; // index to mid vertex point
};

struct Face {
	uint 	num_edges;	// number of edges
	uint 	edge;		// first edge
	uint	vertex;		// vertex index in face
};

struct Surface {
	uint					num_vertices;
	uint					num_edges;
	uint					num_faces;
	union {
		ulong					ul;
		__global struct Vertex 	*v;
	} v;
	union {
		ulong					ul;
		__global struct Edge	*e;
	} e;
	union {
		ulong					ul;
		__global struct Face	*f;
	} f;
	uint					user[4];
};

void print_edge(__global const struct Surface * surface, uint eIdx);

void fix_next_edge(__global const struct Surface * inpSurface,
				   __global struct Surface * outSurface,
				   __global const struct Edge * oldEdge,
				   __global struct Edge * outEdge);

uint edge_reserve(__global struct Surface * surface,
				   uint num_edges);

uint faces_midpoint(__global const struct Surface * inpSurface,
					__global struct Surface * outSurface,
					uint faceIdx);


uint edge_reserve(__global struct Surface * surface,
				   uint num_edges)
{
	return atomic_add(&surface->num_edges, num_edges);
}

void print_edge(__global const struct Surface * surface, uint eIdx) {
	global struct Edge * e = &surface->e.e[eIdx];
	printf("e[%d]: head=%d pair=%d next=%d face=%d\n",eIdx, e->head, e->pair, e->next, e->face);
}

__kernel void surface_init(
					__global struct Surface * surface,
				   	__global struct Vertex * vtxList, unsigned int num_vertices,
				   	__global struct Edge * edgeList, unsigned int num_edges,
					__global struct Face * faceList, unsigned int num_faces)
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


//
// Step 1: For each edge, find midpoint
//		- Split edge in two
// Step 2: For each face, find midpoint
// Step 3: Create new faces:

void fix_next_edge(__global const struct Surface * inpSurface,
				   __global struct Surface * outSurface,
				   __global const struct Edge * oldEdge,
				   __global struct Edge * outEdge)
{
	uint num_edges = inpSurface->num_edges;
	uint nextEdge = oldEdge->next;
	if (nextEdge == UINT_MAX) {
		outEdge->next = UINT_MAX;
	} else if (nextEdge % 2 == 0) {
		outEdge->next = nextEdge + num_edges;
	} else {
		outEdge->next = nextEdge;
	}
}

__kernel void subdivide_edges_midpoint(
					__global const struct Surface * inpSurface,
					__global struct Surface * outSurface)
{
	uint i = get_global_id(0) * 2;
	uint i_pair = i + 1;
	if (i_pair >= inpSurface->num_edges) {
		//printf("i_pair: %d\n",i_pair);
		return;
	}
	if (i == 0) {
		outSurface->num_vertices += (uint)(inpSurface->num_edges / 2);
		outSurface->num_edges += (uint)(inpSurface->num_edges);
	}
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	uint num_edges = inpSurface->num_edges;
	uint outNumVertices = outSurface->num_vertices - (num_edges/2);
	
	uint j = i + num_edges;
	uint j_pair = i_pair + num_edges;
	//printf("i= %d ip=%d j=%d jp=%d\n", i, i_pair, j, j_pair);
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
	global struct Edge * e = &inpSurface->e.e[i];
	global struct Edge * e_pair = &inpSurface->e.e[i_pair];

	float4 p1, p2;
	p1 = inpSurface->v.v[e->head].p;
	p2 = inpSurface->v.v[e_pair->head].p;
	
	uint k = outNumVertices + (i/2); // atomic_inc(&outSurface->user[0]);
	outSurface->v.v[k].p = (p1 + p2)/2.0f;
	outSurface->v.v[k].e = i; // assign edge to point to
	
	// Split edge
	//		Create two new edges
	// |     |     |
	// |__.__|__.__|
	// |     |     |
	// .  .  .  .  .
	// |__.__|__.__|
	//
	// point to the next edge. This is a new edge and it might not exist yet.
	global struct Edge * oe = &outSurface->e.e[i];
	oe->pair = i_pair;
	oe->head = e->head;
	//oe->next = e->next == UINT_MAX ? UINT_MAX : e->next + num_edges;
	fix_next_edge(inpSurface, outSurface, e, oe);
	oe->face = e->face == UINT_MAX ? UINT_MAX : e->face;
	
//	print_edge(outSurface, i);
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
	global struct Edge * oep = &outSurface->e.e[i_pair];
	oep->pair = i;
	oep->head = k;
	oep->next = i_pair + num_edges;
	oep->face = e_pair->face == UINT_MAX ? UINT_MAX : e_pair->face;
	
//	print_edge(outSurface, i_pair);
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
	oe = &outSurface->e.e[j];
	oe->pair = j_pair;
	oe->head = k;
	oe->next = i;
	oe->face = e->face == UINT_MAX ? UINT_MAX : e->face;
	
//	print_edge(outSurface, j);
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	
	oep = &outSurface->e.e[j_pair];
	oep->pair = j;
	oep->head = e_pair->head == UINT_MAX ? UINT_MAX : e_pair->head;
	//oep->next = e_pair->next == UINT_MAX ? UINT_MAX : e_pair->next + num_edges;
	fix_next_edge(inpSurface, outSurface, e_pair, oep);
	oep->face = e_pair->face == UINT_MAX ? UINT_MAX : e_pair->face;
	
//	print_edge(outSurface, j_pair);
	
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
//	printf("v[%d]= %v4f \n",j,outSurface->v.v[j].p);
	
}

__kernel void subdivide_vertices_adjust_pos(
							__global const struct Surface * inpSurface,
							__global struct Surface * outSurface
											)
{
	uint faceIdx = get_global_id(0);
	if (faceIdx >= inpSurface->num_faces) {
		return;
	}
//	if (faceIdx == 0) {
//		printf("Num iterations: %d\n", outSurface->user[0]);
//		outSurface->user[0]++;
//	}
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	global struct Face * f = &inpSurface->f.f[faceIdx];
	
	global struct Edge   *e0 = 0, *e1 = 0, *e2 = 0;
	global struct Vertex *v0 = 0, *v1 = 0, *v2 = 0;
	global struct Vertex *oV1= 0;
	uint eIdx = f->edge;
	e1 = &inpSurface->e.e[eIdx];
	v1 = &inpSurface->v.v[e1->head];
	eIdx = e1->next;
	uint eInit = eIdx;
	do {
		e0 = e1;
		v0 = v1;
		e1 = &inpSurface->e.e[eIdx];
		v1 = &inpSurface->v.v[e1->head];

		eIdx = e1->next;
		e2 = &inpSurface->e.e[eIdx];
		v2 = &inpSurface->v.v[e2->head];

		oV1 = &outSurface->v.v[e1->head];
		float4 p0 = v0->p; // (v0->p + v1->p ) / 2.0f;
		float4 p1 = v2->p; // (v2->p + v1->p ) / 2.0f;
		float alpha = 0.7f;
		float beta = 0.15f;
		float4 avg = (p1 + p0); // p0 + t * (p1 - p0); // p0 - tp0 = p0(1-t)
		oV1->p = (alpha * v1->p) + (beta * avg);
		oV1->p.w = 1.0f;
		// y(x) = a*x^2 + b*x + c
//		printf("V0: %v4f\n", v0->p);
//		printf("V1: %v4f\n", v1->p);
//		printf("V2: %v4f\n", v2->p);
//		printf("oV1: %v4f \n", oV1->p);
		printf("---\n");
	} while (eIdx != eInit);
}

uint faces_midpoint(
				  __global const struct Surface * inpSurface,
				  __global struct Surface * outSurface,
					uint faceIdx)
{
	uint outNumVertices = outSurface->num_vertices - inpSurface->num_faces;
	
	float4 p;
	global struct Face * f = &inpSurface->f.f[faceIdx];
	global struct Edge * e = &inpSurface->e.e[f->edge];
	global struct Edge * e0 = e;

	do {
		p += inpSurface->v.v[e->head].p;
		e = &inpSurface->e.e[e->next];
	} while (e != e0);

	float num_edges = convert_float(f->num_edges);
	p = p / num_edges;
	// copy yhe result to the output surface
	uint midpoint_idx = outNumVertices + faceIdx;
	outSurface->v.v[midpoint_idx].p = p;
	return midpoint_idx;
}

/*
 Edge with even index point to the same vertex in the input and and the output
 Edge with odd index point to midpoint vertex in the output
 */

__kernel void subdivide_add_faces(
							   __global const struct Surface * inpSurface,
							   __global struct Surface * outSurface)
{
	uint faceIdx = get_global_id(0);
	if (faceIdx >= inpSurface->num_faces) {
		return;
	}
	// Reserve the number of points and faces we are going to create
	if (faceIdx == 0) {
		outSurface->num_vertices += inpSurface->num_faces;
		outSurface->num_faces = inpSurface->num_faces * 4;
	}
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	uint face_midpoint = faces_midpoint(inpSurface, outSurface, faceIdx);
	//uint numEdges = inpSurface->num_edges;
	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
	printf("F[%d].midpoint: v[%d]=%v4f\n",
		   faceIdx, face_midpoint, outSurface->v.v[face_midpoint]);
}
