#ifndef POINTRENDERER_H_
#define POINTRENDERER_H_

#include "IRenderer.h"

class PointRenderer : public IRenderer
{
public:
	PointRenderer();
	virtual ~PointRenderer();

	virtual void renderObject(const IObject3D* mesh);
};

#endif /*POINTRENDERER_H_*/
