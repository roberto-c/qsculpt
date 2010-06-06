#ifndef FLATRENDERER_H_
#define FLATRENDERER_H_

#include "IRenderer.h"
#include "BufferObject.h"

class FlatRenderer : public IRenderer
{
public:
	FlatRenderer();
	virtual ~FlatRenderer();

	/**
	 * Render the object using flat shading. The drawing of the mesh is done
	 * by using VBOs if supported or if there are enabled.
	 */
	void renderObject(const ISurface* mesh);
	
private:
	/**
	 * Draw the mesh using OpenGL VBOs.
	 * The VBOs are re-build when the mesh has been changed since the last draw.
	 */
	void renderVbo(const ISurface* mesh);
	
	/**
	 * Draw the mesh using the glBeing()/glEnd() and friends functions.
	 * This method is a fallback method if the  VBOs are not supported.
	 */
	void renderImmediate(const ISurface* mesh);
	
	/**
	 *
	 */
	VertexBuffer* getVBO(ISurface* mesh);
	
	/**
	 * Fill vertex buffer with the mesh data.
	 */
	void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo);
};

#endif /*FLATRENDERER_H_*/
