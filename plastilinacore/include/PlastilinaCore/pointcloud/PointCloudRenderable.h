//
//  PointCloudRenderable.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/30/13.
//
//

#ifndef PlastilinaCore_PointCloudRenderable_h
#define PlastilinaCore_PointCloudRenderable_h

#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>

class VertexBuffer;

class PointCloudRenderable : public IRenderable
{
  public:
    PointCloudRenderable();
    virtual ~PointCloudRenderable();

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
};

#endif
