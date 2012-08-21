#ifndef WIREFRAMERENDERER_H_
#define WIREFRAMERENDERER_H_

#include <PlastilinaCore/IRenderer.h>

class WireframeRenderer : public IRenderer
{
public:
	WireframeRenderer();
	virtual ~WireframeRenderer();

	virtual void renderObject(const ISurface* mesh, const Material * mat);
};

#endif /*WIREFRAMERENDERER_H_*/
