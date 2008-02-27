#ifndef SMOOTHRENDERER_H_
#define SMOOTHRENDERER_H_

#include "IRenderer.h"
#include "BufferObject.h"

class SmoothRenderer : public IRenderer
{
public:
	SmoothRenderer();
	virtual ~SmoothRenderer();

	virtual void renderObject(const IObject3D* mesh);
	
private:
	/**
	 * Draw the mesh using OpenGL VBOs.
	 * The VBOs are re-build when the mesh has been changed since the last draw.
	 */
	void renderVbo(const IObject3D* mesh);
	
	/**
	 * Draw the mesh using the glBeing()/glEnd() and friends functions.
	 * This method is a fallback method if the  VBOs are not supported.
	 */
	void renderImmediate(const IObject3D* mesh);
	
	VertexBuffer m_vertexBuffer;
	VertexBuffer m_normalBuffer;
};

#endif /*SMOOTHRENDERER_H_*/
