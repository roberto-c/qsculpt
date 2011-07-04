#ifndef FLATRENDERER_H_
#define FLATRENDERER_H_

#include "IRenderer.h"

class VertexBuffer;
class Face;

class FlatRenderer : public IRenderer
{
    struct Impl;
    QScopedPointer<Impl> _d;
    
public:
	FlatRenderer();
	virtual ~FlatRenderer();

	/**
	 * Render the object using flat shading. The drawing of the mesh is done
	 * by using VBOs if supported or if there are enabled.
	 */
	void renderObject(const ISurface* mesh);
};

#endif /*FLATRENDERER_H_*/
