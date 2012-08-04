#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/RendererFactory.h>
#include <PlastilinaCore/SpEnums.h>
#include <PlastilinaCore/PointRenderer.h>
#include <PlastilinaCore/FlatRenderer.h>
#include <PlastilinaCore/WireframeRenderer.h>
#include <PlastilinaCore/SmoothRenderer.h>

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