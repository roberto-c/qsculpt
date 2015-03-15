#ifndef RENDERERFACTORY_H_
#define RENDERERFACTORY_H_


#include <PlastilinaCore/SpEnums.h>
#include <PlastilinaCore/IRenderer.h>

class RendererFactory
{
public:
	RendererFactory();
	virtual ~RendererFactory();

	static IRenderer* getRenderer(DrawingMode drawingMode);
};

#endif /*RENDERERFACTORY_H_*/
