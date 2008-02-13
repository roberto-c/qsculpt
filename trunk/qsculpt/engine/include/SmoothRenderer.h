#ifndef SMOOTHRENDERER_H_
#define SMOOTHRENDERER_H_

#include "IRenderer.h"

class SmoothRenderer : public IRenderer
{
public:
	SmoothRenderer();
	virtual ~SmoothRenderer();

	virtual void renderObject(const IObject3D* mesh);
};

#endif /*SMOOTHRENDERER_H_*/
