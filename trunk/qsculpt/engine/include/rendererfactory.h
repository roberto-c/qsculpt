#ifndef RENDERERFACTORY_H_
#define RENDERERFACTORY_H_

#include <QStringList>
#include "spenums.h"
#include "irenderer.h"

class RendererFactory
{
public:
	RendererFactory();
	virtual ~RendererFactory();

	static IRenderer* getRenderer(DrawingMode drawingMode);
};

#endif /*RENDERERFACTORY_H_*/