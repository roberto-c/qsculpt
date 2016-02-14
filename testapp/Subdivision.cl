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
	//printf("VtxSize=%d  edgeSize=%d  faceSize=%d\n", num_vertices, num_edges, num_faces);
}



kernel void build_mesh(
    global struct Vertex * vertices, uint vertex_size,
    global struct Edge * edges, uint edge_size,
    global struct Face * faces, uint face_size,
    global struct GLVertexData * outVertices, uint out_vertex_size
	)
{
	uint gid = get_global_id(0);
	if (gid >= face_size) return;

	if (gid == 0)
	{  
		printf("Vertex size: %d; Edge size: %d; Face size: %d\n", vertex_size, edge_size, face_size);
        printf("OutputSize: %d\n", out_vertex_size);
	}
    uint faceOutIdx = gid * 3 * 2; // Triangle: 3 vertices, two triangles

	struct Surface surface;
	surface.num_vertices = vertex_size;
	surface.v.v = vertices;
	surface.num_edges = edge_size;
	surface.e.e = edges;
	surface.num_faces = face_size;
	surface.f.f = faces;

	global struct Face *face = &faces[gid];
	Face_print(face);
	struct Face_VertexIterator it;
	Face_VertexIterator_init(&it, &surface, face);
	
	uint face_vtx_counter = 0;
	global struct Vertex* vtx1 = Face_VertexIterator_value(&it);
    global struct Vertex* vtx2 = Face_VertexIterator_next(&it);
    global struct Vertex* vtx3 = Face_VertexIterator_next(&it);
    while (vtx3)
    {
        face_vtx_counter++;
        outVertices[faceOutIdx + 0].p = vtx1->p;
        outVertices[faceOutIdx + 0].c = vtx1->c;
        outVertices[faceOutIdx + 0].n = vtx1->n;
        outVertices[faceOutIdx + 0].t = vtx1->t;
        outVertices[faceOutIdx + 1].p = vtx2->p;
        outVertices[faceOutIdx + 1].c = vtx2->c;
        outVertices[faceOutIdx + 1].n = vtx2->n;
        outVertices[faceOutIdx + 1].t = vtx2->t;
        outVertices[faceOutIdx + 2].p = vtx3->p;
        outVertices[faceOutIdx + 2].c = vtx3->c;
        outVertices[faceOutIdx + 2].n = vtx3->n;
        outVertices[faceOutIdx + 2].t = vtx3->t;
        outVertices += 3;
        vtx2 = vtx3;
        vtx3 = Face_VertexIterator_next(&it);
    };
    //Face_VertexIterator_init(&it, &surface, face);
    //vtx1 = Face_VertexIterator_value(&it);
    //while (vtx1)
    //{
    //    face_vtx_counter++;
    //    vtx1 = Face_VertexIterator_next(&it);
    //}
	printf("Face[%d]: NumVertices: %d\n", Face_iid(face), face_vtx_counter);
    return;
}
