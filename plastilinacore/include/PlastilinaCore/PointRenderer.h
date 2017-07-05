#ifndef POINTRENDERER_H_
#define POINTRENDERER_H_

#include <PlastilinaCore/IRenderer.h>

/**
 * Renders surface object as point meshes.
 */
class PointRenderer : public IRenderer
{
  public:
    PointRenderer();

    /**
     * Constructor of point renderer. Creates new buffer objects under
     * the specified poolname.
     */
    PointRenderer(const std::string& poolname);

    virtual ~PointRenderer();

    virtual void renderObject(std::shared_ptr<SceneNode>& node);

    /**
     * Set the point size to use when rendering points
     */
    void setPointSize(float size);

    /**
     * Retuns the point size;
     */
    float pointSize();

  private:
    struct Impl;
    std::unique_ptr<Impl> d_;
};

#endif /*POINTRENDERER_H_*/
