#include <PlastilinaCore/Stable.h>
#include <cstddef>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/GlslShader.h>
#include <PlastilinaCore/GlslProgram.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/WireframeRenderer.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>
#define BO_POOL_NAME "WireframeRendererPool"

static GLfloat g_selectedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
static GLfloat g_normalColor[] =   {0.8f, 0.8f, 0.8f, 1.0f};

typedef struct tagFlatVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat color[4];
} FlatVtxStruct;

class RendererPrivate
{
public:
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last draw.
     */
	static void renderObject(std::shared_ptr<SceneNode> & node);
	
	static VAO* getVAO(ISurface* mesh);
    /**
     *
     */
    static VertexBuffer* getVBO(ISurface* mesh);

    /**
     * Fill vertex buffer with the mesh data.
     */
    static void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo);
};

WireframeRenderer::WireframeRenderer()
{
    std::cerr << "WireframeRenderer constructor" << std::endl;
}

WireframeRenderer::~WireframeRenderer()
{
    std::cerr << "WireframeRenderer destructor" << std::endl;
}

void WireframeRenderer::renderObject(std::shared_ptr<SceneNode> & node)
{
	RendererPrivate::renderObject(node);
}

void RendererPrivate::renderObject(std::shared_ptr<SceneNode> & node)
{
	ISurface * obj = NULL;
	std::shared_ptr<Material> mat;
	
	if (!node) {
		return;
	}
	SurfaceNode::shared_ptr snode = std::dynamic_pointer_cast<SurfaceNode>(node);
	if (!snode) {
		std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
		return;
	}
	obj = snode->surface();
	mat = snode->material();

	//std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (obj == NULL || mat == NULL)
        return;
    
    VertexBuffer* vbo = getVBO(obj);
    if (vbo == NULL || vbo->objectID() == 0)
    {
        std::cerr << "Failed to create VBO. Fallback to immediate mode" << std::endl ;
        return;
    }
	
	VAO* vao = getVAO(obj);
    if (vao == NULL || vao->objectID() == 0)
	{
		std::cerr << "Failed to create VAO."  << std::endl;
		return;
	}
	
    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);
    
	vao->bind();
    vbo->bind();
    if ( vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);
		
		GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
		if (attColor >= 0) {
			glEnableVertexAttribArray(attColor);
			glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
								  sizeof(FlatVtxStruct),
								  (GLvoid*)offsetof(FlatVtxStruct, color));
		}
		GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
		if (attVtx >= 0) {
			glEnableVertexAttribArray(attVtx);
			glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
								  sizeof(FlatVtxStruct),
								  (GLvoid*)offsetof(FlatVtxStruct, v));
		}
		GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
		if (attNormal >= 0) {
			glEnableVertexAttribArray(attNormal);
			glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
								  sizeof(FlatVtxStruct),
								  (GLvoid*)offsetof(FlatVtxStruct, n));
		}
		THROW_IF_GLERROR("Failed to get attribute");
    }
	
	mat->shaderProgram()->useProgram();
    GLsizei numVertices = vbo->getBufferSize() / sizeof(FlatVtxStruct);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    vao->release();
        
    //std::cerr << "Mesh rendered";
}

VAO* RendererPrivate::getVAO(ISurface* mesh)
{
	VAO* vao = NULL;
	vao = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
	if (vao == NULL)
	{
		vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
	}
	return vao;
}

VertexBuffer* RendererPrivate::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

void RendererPrivate::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    //std::cerr << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;
    
    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;
    
    size_t numVertices = numFaces*4;
    FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
    
    //    int fcounter = 0;
    int offset = 0;
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        auto f = it.next();
        //        std::cerr << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f->constVertexIterator();
        while(vtxIt.hasNext()) {
            auto v = vtxIt.next();
            vtxData[offset].v[0] = v->position().x();
            vtxData[offset].v[1] = v->position().y();
            vtxData[offset].v[2] = v->position().z();
			vtxData[offset].v[3] = 1;
            
            vtxData[offset].n[0] = v->normal().x();
            vtxData[offset].n[1] = v->normal().y();
            vtxData[offset].n[2] = v->normal().z();
			vtxData[offset].n[3] = 0;
            
            if (f->flags() & FF_Selected) {
                memcpy(vtxData[offset].color, g_selectedColor,
                       sizeof(g_selectedColor)) ;
            }
            else
            {
                memcpy(vtxData[offset].color, g_normalColor,
                       sizeof(g_normalColor)) ;
            }
            offset++;
        }
    }
    GLuint bufferSize = static_cast<GLuint>(numVertices*sizeof(FlatVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData, bufferSize);
    
    delete [] vtxData;
    
    //std::cerr << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}