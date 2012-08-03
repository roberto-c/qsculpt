#ifndef WIREFRAMERENDERER_H_
#define WIREFRAMERENDERER_H_

#include <CoreEngine/IRenderer.h>

class WireframeRenderer : public IRenderer
{
public:
	WireframeRenderer();
	virtual ~WireframeRenderer();

	virtual void renderObject(const ISurface* mesh);
};

#endif /*WIREFRAMERENDERER_H_*/
