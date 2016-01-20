#pragma once

#if defined(__OPENCL_VERSION__)
#define alignas(x) __attribute__ ((aligned (x)))
#else
#include <PlastilinaCore/opencl/OpenCL.h>
#define float2  cl_float2
#define float4  cl_float4
#define uint    cl_uint
#define ulong   cl_ulong
#define global
#define __global
#endif

struct alignas(16) Vertex {
#if defined(__OPENCL_VERSION__)
    uint    iid; // pointid
#endif
    float4 	p;	// position
    float4 	n;	// normal
    float4  c;  // color
    float2  t;  // texture
    uint	e;	// edge index
    uint    f;  // face index

#if defined(__cplusplus)
    Vertex()
        : p({ 0.f,0.f,0.f,0.f })
        , n({ 0.f,0.f,0.f,0.f })
        , c({ 0.f,0.f,0.f,0.f })
        , t({ 0.f,0.f})
        , e(0)
        , f(0)
    {}
    Vertex(const Vertex& cpy) = default;
    Vertex(Vertex&& cpy) = default;
#endif
};

struct Edge {
#if defined(__OPENCL_VERSION__)
    uint    iid; // pointid
#endif
    uint	head;	// index to vertex array
    uint 	pair; 	// index to edge array
    uint 	face;	// index to a face
    uint 	next;	// next edge when forming a face
                    //	uint 	midpoint; // index to mid vertex point
};

struct Face {
#if defined(__OPENCL_VERSION__)
    uint    iid;        // pointid
#endif
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
