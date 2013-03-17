#ifndef IRENDERER_H_
#define IRENDERER_H_

class ISurface;
class Material;
class SceneNode;
class Camera;

class IRenderer
{
public:
	virtual ~IRenderer() {}

	/**
	 * This method must be implemented by a derived class. This method
	 * dispatches the commands need it to render objects on the display.
	 */
	virtual void renderObject(std::shared_ptr<SceneNode> & node) = 0;
};

enum RenderMode {
	RM_WireFrame,
	RM_Flat,
	RM_Smooth,
	RM_Textured
};

struct RenderState {
	const SceneNode 	*root;
	const SceneNode 	*currentNode;
	RenderMode  		renderMode;
	Camera 				*camera;
};

class IRenderable
{
public:
	virtual ~IRenderable(){};
	
	virtual void render(const RenderState * state) const = 0;
};

#endif /*IRENDERER_H_*/
