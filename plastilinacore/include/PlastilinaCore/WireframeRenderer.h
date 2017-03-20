#ifndef WIREFRAMERENDERER_H_
#define WIREFRAMERENDERER_H_

#include <PlastilinaCore/IRenderer.h>

class WireframeRenderer : public IRenderer
{
  public:
    WireframeRenderer();
    virtual ~WireframeRenderer();

    virtual void renderObject(std::shared_ptr<SceneNode>& node);
};

#endif /*WIREFRAMERENDERER_H_*/
