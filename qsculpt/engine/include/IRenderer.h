#ifndef IRENDERER_H_
#define IRENDERER_H_

class IObject3D;

class IRenderer
{
public:
	virtual ~IRenderer() {}

	/**
	 * This method must be implemented by a derived class. This method
	 * dispatches the commands need it to render objects on the display.
	 */
	virtual void renderObject(const IObject3D* mesh) = 0;
};

#endif /*IRENDERER_H_*/
