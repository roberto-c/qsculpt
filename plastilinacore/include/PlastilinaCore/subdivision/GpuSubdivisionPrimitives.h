#pragma once

#if defined(__OPENCL_VERSION__)
#define alignas(x) __attribute__((aligned(x)))
#else
#include <PlastilinaCore/opencl/OpenCL.h>
#define float2 cl_float2
#define float4 cl_float4
#define uint cl_uint
#define ulong cl_ulong
#define global
#define __global
#endif

struct alignas(16) GLVertexData
{
    float4 p;
    float4 n;
    float4 c;
    float2 t;
    float2 padding;
};

struct alignas(16) Vertex
{
#if defined(__OPENCL_VERSION__)
    uint iid; // pointid
#endif
    float4 p; // position
    float4 n; // normal
    float4 c; // color
    float2 t; // texture
    uint   e; // edge index
    uint   f; // face index

#if defined(__cplusplus)
    Vertex()
        : p({0.f, 0.f, 0.f, 0.f})
        , n({0.f, 0.f, 0.f, 0.f})
        , c({0.f, 0.f, 0.f, 0.f})
        , t({0.f, 0.f})
        , e(0)
        , f(0)
    {
    }
    Vertex(const Vertex& cpy) = default;
    Vertex(Vertex&& cpy)      = default;
#endif
};

struct Edge
{
#if defined(__OPENCL_VERSION__)
    uint iid; // pointid
#endif
    uint head; // index to vertex array
    uint pair; // index to edge array
    uint face; // index to a face
    uint next; // next edge when forming a face
               //	uint 	midpoint; // index to mid vertex point
};

struct Face
{
#if defined(__OPENCL_VERSION__)
    uint iid; // pointid
#endif
    uint num_edges; // number of edges
    uint edge;      // first edge
    uint vertex;    // vertex index in face
};

struct Surface
{
    uint num_vertices;
    uint num_edges;
    uint num_faces;
    union {
        ulong                   ul;
        __global struct Vertex* v;
    } v;
    union {
        ulong                 ul;
        __global struct Edge* e;
    } e;
    union {
        ulong                 ul;
        __global struct Face* f;
    } f;
    uint user[4];
};

#if defined(__OPENCL_VERSION__)
// opencl iterator functions
inline uint Vertex_iid(global struct Vertex* v) { return v->iid & 0xFFFFFF; }
inline uint Edge_iid(global struct Edge* e) { return e->iid & 0xFFFFFF; }
inline uint Face_iid(global struct Face* f) { return f->iid & 0xFFFFFF; }

inline global struct Vertex* Surface_vertex(struct Surface* surface,
                                            uint            vtxIid)
{
    return &surface->v.v[vtxIid - 1];
}

inline global struct Edge* Surface_edge(struct Surface* surface, uint edgeIid)
{
    return &surface->e.e[edgeIid - 1];
}

inline global struct Face* Surface_face(struct Surface* surface, uint faceIid)
{
    return &surface->f.f[faceIid - 1];
}

void Edge_print(global struct Edge* e)
{
    if (!e)
        printf("Edge[NULL]\n");

    printf("Edge[%d]: Next=%d, Pair=%d, VtxHead=%d\n", Edge_iid(e), e->next,
           e->pair, e->head);
}

void Face_print(global struct Face* e)
{
    if (!e)
        printf("Edge[NULL]\n");

    printf("Face[%d]: Edge=%d, Vertex=%d, NumEdges=%d\n", Face_iid(e),
           e->edge, e->vertex, e->num_edges);
}

struct Face_VertexIterator
{
    struct Surface*     surface;
    global struct Face* face;
    global struct Edge* current;
};

inline void Face_VertexIterator_init(struct Face_VertexIterator* it,
                                     struct Surface*             surface,
                                     global struct Face*         face)
{
    it->surface = surface;
    it->face    = face;
    it->current = Surface_edge(it->surface, face->edge);
}

inline global struct Vertex*
Face_VertexIterator_value(struct Face_VertexIterator* it)
{
    return Surface_vertex(it->surface, it->current->head);
}

inline global struct Vertex*
Face_VertexIterator_next(struct Face_VertexIterator* it)
{
    uint next_iid = it->current->next;
    if (next_iid == 0)
    {
        return NULL;
    }

    global struct Edge* next = Surface_edge(it->surface, next_iid);
    it->current              = next;
    if (next->head == 0)
        return NULL;
    return Surface_vertex(it->surface, it->current->head);
}
#endif
