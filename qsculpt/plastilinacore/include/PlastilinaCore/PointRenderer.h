#ifndef POINTRENDERER_H_
#define POINTRENDERER_H_

#include <CoreEngine/IRenderer.h>
#include <CoreEngine/BufferObject.h>

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
    PointRenderer(const std::string & poolname);
    
	virtual ~PointRenderer();

	virtual void renderObject(const ISurface* mesh);
    
    /**
     * Set the point size to use when rendering points
     */
    void setPointSize(float size);
    
    /**
     * Retuns the point size;
     */
    float pointSize();
	
private:
    class Impl;
    std::unique_ptr<Impl> d_;
    
};

#endif /*POINTRENDERER_H_*/
