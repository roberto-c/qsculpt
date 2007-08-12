#ifndef IRENDERER_H_
#define IRENDERER_H_

class IObject3D;

class IRenderer
{
public:
	virtual ~IRenderer() {}

	/**
	 *
	 */
	virtual void renderObject(const IObject3D* mesh) = 0;
};

#endif /*IRENDERER_H_*/
