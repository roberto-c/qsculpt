#include "stdafx.h"
#include "rendererfactory.h"
#include "spenums.h"
#include "pointrenderer.h"
#include "flatrenderer.h"
#include "wireframerenderer.h"
#include "smoothrenderer.h"

RendererFactory::RendererFactory()
{
}

RendererFactory::~RendererFactory()
{
}

IRenderer* RendererFactory::getRenderer(DrawingMode drawingMode)
{
	switch(drawingMode)
	{
	case Points:
		return new PointRenderer;
		break;

	case Wireframe:
		return new WireframeRenderer;
		break;

	case Flat:
		return new FlatRenderer;
		break;

	case Smooth:
		return new SmoothRenderer;
		break;

	case Texture:
	default:
		return new FlatRenderer;
	}
}