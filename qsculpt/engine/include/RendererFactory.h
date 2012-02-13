#ifndef RENDERERFACTORY_H_
#define RENDERERFACTORY_H_

#include <QtCore/QStringList>
#include "SpEnums.h"
#include "IRenderer.h"

class RendererFactory
{
public:
	RendererFactory();
	virtual ~RendererFactory();

	static IRenderer* getRenderer(DrawingMode drawingMode);
};

#endif /*RENDERERFACTORY_H_*/
