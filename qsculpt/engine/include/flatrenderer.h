#ifndef FLATRENDERER_H_
#define FLATRENDERER_H_

#include "irenderer.h"

class FlatRenderer : public IRenderer
{
public:
	FlatRenderer();
	virtual ~FlatRenderer();

	void renderObject(const IObject3D* mesh);
};

#endif /*FLATRENDERER_H_*/
