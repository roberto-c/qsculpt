#ifndef FLATRENDERER_H_
#define FLATRENDERER_H_

#include <PlastilinaCore/IRenderer.h>

class VertexBuffer;
class Face;
class GlslProgram;

class FlatRenderer : public IRenderer
{
    struct Impl;
    std::unique_ptr<Impl> _d;
    
public:
	FlatRenderer();
	virtual ~FlatRenderer();

	/**
	 * Render the object using flat shading. The drawing of the mesh is done
	 * by using VBOs if supported or if there are enabled.
	 */
	virtual void renderObject(std::shared_ptr<SceneNode> & node);
    
    /**
     * Set the program shader used for flat rendering. If no set, then it will use
     * fixed pipeline rendering.
     */
    void setShaderProgram(GlslProgram * shader);
};

#endif /*FLATRENDERER_H_*/
