#include "StdAfx.h"
#include "RendererFactory.h"
#include "SpEnums.h"
#include "PointRenderer.h"
#include "FlatRenderer.h"
#include "WireframeRenderer.h"
#include "SmoothRenderer.h"

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