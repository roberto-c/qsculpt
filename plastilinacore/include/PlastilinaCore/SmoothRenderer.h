#ifndef SMOOTHRENDERER_H_
#define SMOOTHRENDERER_H_

#include <PlastilinaCore/BufferObject.h>
#include <PlastilinaCore/IRenderer.h>

class SmoothRenderer : public IRenderer
{
    struct Impl;
    std::unique_ptr<Impl> _d;

  public:
    SmoothRenderer();
    virtual ~SmoothRenderer();

    virtual void renderObject(std::shared_ptr<SceneNode>& node);
};

#endif /*SMOOTHRENDERER_H_*/
