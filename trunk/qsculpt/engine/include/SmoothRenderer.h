#ifndef SMOOTHRENDERER_H_
#define SMOOTHRENDERER_H_

#include "IRenderer.h"
#include "BufferObject.h"



class SmoothRenderer : public IRenderer
{
    struct Impl;
    QScopedPointer<Impl> _d;
    
public:
	SmoothRenderer();
	virtual ~SmoothRenderer();

	virtual void renderObject(const ISurface* mesh);	
};

#endif /*SMOOTHRENDERER_H_*/
