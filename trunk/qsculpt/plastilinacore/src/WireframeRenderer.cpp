#include <PlastilinaCore/Stable.h>
#include <cstddef>
#include <PlastilinaCore/WireframeRenderer.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>

#define BO_POOL_NAME "WireframeRendererPool"

static GLfloat g_selectedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
static GLfloat g_normalColor[] =   {0.8f, 0.8f, 0.8f, 1.0f};

typedef struct tagFlatVtxStruct
{
    GLfloat v[3];
    GLfloat n[3];
    GLfloat color[4];
} FlatVtxStruct;

class RendererPrivate
{
public:
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last draw.
     */
    static void renderVbo(const ISurface* mesh, const Material * mat);

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
    std::cerr << "WireframeRenderer constructor";
}

WireframeRenderer::~WireframeRenderer()
{
    std::cerr << "WireframeRenderer destructor";
}

void WireframeRenderer::renderObject(const ISurface* mesh, const Material * mat)
{
    RendererPrivate::renderVbo(mesh, mat);
}

void RendererPrivate::renderVbo(const ISurface* mesh, const Material * mat)
{
    //std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;
    
    ISurface* obj = const_cast<ISurface*>(mesh);
    
    VertexBuffer* vbo = getVBO(obj);
    if (vbo == NULL || vbo->objectID() == 0)
    {
        std::cerr << "Failed to create VBO. Fallback to immediate mode" ;
        return;
    }
    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);
    
    if ( vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);
    }
    
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_NORMAL_ARRAY);
//    glEnableClientState(GL_COLOR_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo->objectID());
//    glColorPointer(4, GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, color));
//    glNormalPointer(GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, n));
//    glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, v));
    
//    Color color(1,1,1,1);
//    if (mesh->isSelected())
//        glColor3d(color.r(), color.g() + 0.3, color.b());
//    else
//        glColor3d(color.r(), color.g(), color.b());
    
    //std::cerr << "Draw mesh";
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINES, 0, (GLsizei)obj->numFaces()*4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);
//    glDisableClientState(GL_COLOR_ARRAY);
    
    //std::cerr << "Mesh rendered";
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
            
            vtxData[offset].n[0] = v->normal().x();
            vtxData[offset].n[1] = v->normal().y();
            vtxData[offset].n[2] = v->normal().z();
            
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