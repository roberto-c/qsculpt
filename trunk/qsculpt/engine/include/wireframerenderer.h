#ifndef WIREFRAMERENDERER_H_
#define WIREFRAMERENDERER_H_

#include "irenderer.h"

class WireframeRenderer : public IRenderer
{
public:
	WireframeRenderer();
	virtual ~WireframeRenderer();

	virtual void renderObject(const IObject3D* mesh);
};

#endif /*WIREFRAMERENDERER_H_*/
