//
//  SubdivisionRenderable.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 3/10/13.
//
//

#ifndef __PlastilinaCore__SubdivisionRenderable__
#define __PlastilinaCore__SubdivisionRenderable__

#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <iostream>

class VertexBuffer;
struct SmoothVtxStruct;

class SubdivisionRenderable : public IRenderable
{

  public:
    SubdivisionRenderable(const Subdivision* surface);

    virtual ~SubdivisionRenderable();

    virtual void render(RenderState& state) const;

  private:
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last
     * draw.
     */
    void renderObject(RenderState& state) const;

    /**
     *
     */
    VertexBuffer* getVBO(ISurface* mesh) const;

    /**
     *
     */
    VAO* getVAO(ISurface* mesh) const;

    /**
     *
     */
    void fillVertexBufferPoints(ISurface* mesh, VertexBuffer* vbo) const;

    /**
     * Fill vertex buffer with the mesh data.
     */
    void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo) const;

    bool processPolygon(const core::subdivision::Face& f,
                        std::vector<SmoothVtxStruct>&  buffer,
                        size_t&                        pos) const;

    bool processTriangle(const core::subdivision::Vertex& v1,
                         const core::subdivision::Vertex& v2,
                         const core::subdivision::Vertex& v3,
                         std::vector<SmoothVtxStruct>&    buffer,
                         size_t&                          pos) const;
};

#endif /* defined(__PlastilinaCore__SubdivisionRenderable__) */
