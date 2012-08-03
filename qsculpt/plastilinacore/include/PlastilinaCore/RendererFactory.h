#ifndef RENDERERFACTORY_H_
#define RENDERERFACTORY_H_


#include <CoreEngine/SpEnums.h>
#include <CoreEngine/IRenderer.h>

class RendererFactory
{
public:
	RendererFactory();
	virtual ~RendererFactory();

	static IRenderer* getRenderer(DrawingMode drawingMode);
};

#endif /*RENDERERFACTORY_H_*/
