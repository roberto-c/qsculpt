#ifndef SMOOTHRENDERER_H_
#define SMOOTHRENDERER_H_

#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/BufferObject.h>



class SmoothRenderer : public IRenderer
{
    struct Impl;
    std::unique_ptr<Impl> _d;
    
public:
	SmoothRenderer();
	virtual ~SmoothRenderer();

	virtual void renderObject(const ISurface* mesh);	
};

#endif /*SMOOTHRENDERER_H_*/
