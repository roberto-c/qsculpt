#ifndef POINTRENDERER_H_
#define POINTRENDERER_H_

#include "IRenderer.h"
#include "BufferObject.h";

class PointRenderer : public IRenderer
{
public:
	PointRenderer();
	virtual ~PointRenderer();

	virtual void renderObject(const ISurface* mesh);
	
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
	
	void fillVertexBuffer(const ISurface* mesh, VertexBuffer* vbo);
};

#endif /*POINTRENDERER_H_*/
